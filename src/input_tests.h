#pragma once
#include <cstdio>
// Real SDL2 virtual devices exercise the same APIs used by physical controllers.
inline int inputTests(Game&game) {
    SDL_Joystick*raw=nullptr,*mapped=nullptr;
    auto &c=game.controls;
    std::string testPath="pf-input-test-"+std::to_string(SDL_GetPerformanceCounter())+".cfg";
    try {
        c.close();c.testVirtualOnly=true;c.preferred[0]="old-disconnected-guid";c.preferred[1]="-";c.path=testPath;
        if(std::string(SDL_GetPlatform())=="Windows") {
            char*mapping=SDL_GameControllerMappingForGUID(SDL_JoystickGetGUIDFromString("03000000c82d00000360000000000000"));
            bool found=mapping && std::string(mapping).find("8BitDo Pro 2")!=std::string::npos;
            SDL_free(mapping);check(found,"Bundled 8BitDo Pro 2 mapping is accepted by SDL2");
        }
        SDL_VirtualJoystickDesc desc{};desc.version=SDL_VIRTUAL_JOYSTICK_DESC_VERSION;
        desc.type=SDL_JOYSTICK_TYPE_UNKNOWN;desc.naxes=4;desc.nbuttons=12;desc.nhats=1;desc.name="PF Virtual Generic";
        int rawIndex=SDL_JoystickAttachVirtualEx(&desc);check(rawIndex>=0,"Attach raw joystick");
        raw=SDL_JoystickOpen(rawIndex);check(raw!=nullptr,"Open raw joystick");
        SDL_JoystickSetVirtualAxis(raw,3,32767);SDL_JoystickUpdate();
        game.focused=true;game.events();check(c.device(0) && !c.device(0)->controller,"Hotplug assigns raw generic to P1");
        auto rawID=SDL_JoystickInstanceID(raw);
        std::array<Uint8,SDL_NUM_SCANCODES> keyboard{};
        SDL_JoystickSetVirtualButton(raw,2,1);SDL_JoystickUpdate();
        check(c.input(0,keyboard.data()).punch && !c.input(1,keyboard.data()).punch,"Generic button and P1 isolation");
        SDL_JoystickSetVirtualButton(raw,2,0);SDL_JoystickSetVirtualHat(raw,0,SDL_HAT_LEFT);SDL_JoystickUpdate();
        check(c.input(0,keyboard.data()).left,"Generic D-pad hat");
        SDL_JoystickSetVirtualHat(raw,0,SDL_HAT_CENTERED);SDL_JoystickUpdate();
        check(!c.input(0,keyboard.data()).left,"Released direction clears");
        c.setBinding(0,controls::Magic,{controls::Axis,2,1});
        SDL_JoystickSetVirtualAxis(raw,2,5000);SDL_JoystickUpdate();check(!c.input(0,keyboard.data()).magic,"Axis deadzone filters drift");
        SDL_JoystickSetVirtualAxis(raw,2,29000);SDL_JoystickUpdate();check(c.input(0,keyboard.data()).magic,"Generic axis binding");
        SDL_JoystickSetVirtualAxis(raw,2,0);SDL_JoystickUpdate();
        game.menu=true;game.menuChoice=1;game.menuAccept();
        check(game.lobby && game.local && !c.device(0) && c.device(1),"Explicit two-player option assigns lone guest pad to P2");
        game.lobbyNav(controls::AcceptNav);game.match.intro=0;
        check(game.versusActive,"Local match first shows versus presentation");
        auto confirm=c.profile(1)->bind[controls::Jump];check(confirm.kind==controls::Button,"Generic confirm button exists");
        SDL_JoystickSetVirtualButton(raw,confirm.index,1);SDL_JoystickUpdate();game.events();
        check(!game.versusActive && !game.paused,"Guest controller can skip versus without pausing fight");
        SDL_JoystickSetVirtualButton(raw,confirm.index,0);SDL_JoystickUpdate();game.events();
        float before=game.match.f[1].x;
        SDL_JoystickSetVirtualHat(raw,0,SDL_HAT_RIGHT);SDL_JoystickUpdate();game.step(1.f/60);
        check(game.local && game.match.f[1].x>before,"Local P2 follows guest pad instead of CPU");
        SDL_JoystickSetVirtualHat(raw,0,SDL_HAT_CENTERED);SDL_JoystickUpdate();
        c.assign(0,rawID);c.preferred[1].clear();game.menu=true;game.local=false;
        // Trigger at +32767 rest must bind the negative direction and release cleanly.
        game.settings.player=0;game.settings.row=3+controls::Ultimate;game.settings.padColumn=true;game.settings.beginCapture(c);
        SDL_Event trigger{};trigger.type=SDL_JOYAXISMOTION;trigger.jaxis.which=rawID;trigger.jaxis.axis=3;trigger.jaxis.value=-32768;
        game.settings.captureEvent(c,trigger);check(!game.settings.capturing,"Raw inverted trigger capture");
        SDL_JoystickSetVirtualAxis(raw,3,-32768);SDL_JoystickUpdate();check(c.input(0,keyboard.data()).ultimate,"Inverted trigger fires action");
        SDL_JoystickSetVirtualAxis(raw,3,32767);SDL_JoystickUpdate();check(!c.input(0,keyboard.data()).ultimate,"Inverted trigger releases without stuck action");
        // PS2 adapters have no universal numbering: calibrate a deliberately shuffled raw pad.
        auto originalPS2=*c.profile(0);
        game.settings.beginPS2(c);
        SDL_Event cancelPS2{};cancelPS2.type=SDL_KEYDOWN;cancelPS2.key.keysym.sym=SDLK_ESCAPE;
        game.settings.captureEvent(c,cancelPS2);
        check(!c.profile(0)->ps2 && !game.settings.capturing,"PS2 wizard cancellation preserves profile");
        game.settings.beginPS2(c);
        controls::Binding ps2Bindings[]={
            {controls::Hat,0,SDL_HAT_LEFT},{controls::Hat,0,SDL_HAT_RIGHT},
            {controls::Button,1,0},{controls::Button,4,0},{controls::Button,3,0},
            {controls::Button,2,0},{controls::Button,5,0},{controls::Button,10,0},
            {controls::Button,0,0},{controls::Button,7,0},{controls::Button,9,0},
            {controls::Axis,2,-1}};
        for(auto binding:ps2Bindings) {
            SDL_Event e{};
            if(binding.kind==controls::Button){SDL_JoystickSetVirtualButton(raw,binding.index,1);e.type=SDL_JOYBUTTONDOWN;e.jbutton.which=rawID;e.jbutton.button=Uint8(binding.index);}
            else if(binding.kind==controls::Hat){SDL_JoystickSetVirtualHat(raw,0,Uint8(binding.value));e.type=SDL_JOYHATMOTION;e.jhat.which=rawID;e.jhat.value=Uint8(binding.value);}
            else {SDL_JoystickSetVirtualAxis(raw,2,-32768);e.type=SDL_JOYAXISMOTION;e.jaxis.which=rawID;e.jaxis.axis=2;e.jaxis.value=-32768;}
            SDL_JoystickUpdate();game.settings.captureEvent(c,e);
            SDL_JoystickSetVirtualHat(raw,0,0);SDL_JoystickSetVirtualAxis(raw,2,0);
            for(int button=0;button<12;button++)SDL_JoystickSetVirtualButton(raw,button,0);
            SDL_JoystickUpdate();SDL_Event release{};if(game.settings.capturing)game.settings.captureEvent(c,release);
        }
        check(c.profile(0)->ps2 && !game.settings.capturing,"PS2 wizard completes with buttons, hat and alternate axis");
        SDL_JoystickSetVirtualButton(raw,7,1);SDL_JoystickUpdate();
        check(c.input(0,keyboard.data()).grab && !c.input(0,keyboard.data()).kick,"Calibrated PS2 R2 grabs without duplicate action");
        SDL_JoystickSetVirtualButton(raw,7,0);SDL_JoystickUpdate();c.navigation();
        SDL_JoystickSetVirtualButton(raw,1,1);SDL_JoystickUpdate();
        auto ps2Nav=c.navigation();check(ps2Nav[controls::AcceptNav] && !ps2Nav[controls::BackNav],"Calibrated PS2 cross confirms menus");
        SDL_JoystickSetVirtualButton(raw,1,0);SDL_JoystickSetVirtualAxis(raw,2,-32768);SDL_JoystickUpdate();
        check(c.navigation()[controls::UpNav] && c.input(0,keyboard.data()).jump,"Calibrated PS2 D-pad up uses alternate axis");
        SDL_JoystickSetVirtualAxis(raw,2,0);SDL_JoystickUpdate();
        controls::System ps2Saved;ps2Saved.path=testPath;
        check(ps2Saved.load(),"PS2 calibration reloads from disk");
        bool persistedPS2=false;for(auto &entry:ps2Saved.profiles)persistedPS2|=entry.second.ps2 && entry.second.up==ps2Bindings[11];
        check(persistedPS2,"PS2 menu direction persists alongside action bindings");
        *c.profile(0)=originalPS2;c.navigation();
        check(c.setKey(0,controls::Punch,SDL_SCANCODE_F),"Keyboard remap");
        keyboard[SDL_SCANCODE_F]=1;check(c.input(0,keyboard.data()).punch,"Remapped key drives gameplay");
        keyboard[SDL_SCANCODE_F]=0;keyboard[SDL_SCANCODE_J]=1;check(!c.input(0,keyboard.data()).punch,"Old key is released from action");keyboard.fill(0);
        check(!c.setKey(0,controls::Punch,SDL_SCANCODE_F1),"Reserved key rejected");
        check(!c.setKey(0,controls::Punch,SDL_SCANCODE_LEFT),"Cross-player duplicate rejected");
        // Capture through the actual settings UI using SDL events.
        game.settings.open=true;game.settings.row=3+controls::Kick;game.settings.player=0;game.settings.padColumn=true;
        game.settings.beginCapture(c);SDL_Event event{};event.type=SDL_JOYBUTTONDOWN;event.jbutton.which=rawID;event.jbutton.button=6;
        game.settings.captureEvent(c,event);check(!game.settings.capturing && c.profile(0)->bind[controls::Kick].index==6,"UI captures raw button");
        game.settings.beginCapture(c);event={};event.type=SDL_KEYDOWN;event.key.keysym.sym=SDLK_ESCAPE;game.settings.captureEvent(c,event);check(!game.settings.capturing,"Escape cancels capture");
        game.settings.open=false;
        desc={};desc.version=SDL_VIRTUAL_JOYSTICK_DESC_VERSION;desc.type=SDL_JOYSTICK_TYPE_GAMECONTROLLER;
        desc.naxes=6;desc.nbuttons=SDL_CONTROLLER_BUTTON_MAX;desc.nhats=1;desc.name="PF Virtual DualShock 4";
        desc.vendor_id=0x054c;desc.product_id=0x05c4;
        int mappedIndex=SDL_JoystickAttachVirtualEx(&desc);check(mappedIndex>=0,"Attach mapped gamepad");
        mapped=SDL_JoystickOpen(mappedIndex);check(mapped!=nullptr,"Open mapped gamepad");
        game.events();check(c.device(1) && c.device(1)->controller,"Hotplug assigns standardized gamepad to P2");
        check(c.device(1)->playstation && c.bindingLabel(1,controls::Punch)=="QUADRADO","SDL recognizes PlayStation identifiers and labels");
        auto mappedID=SDL_JoystickInstanceID(mapped);
        SDL_JoystickSetVirtualButton(mapped,SDL_CONTROLLER_BUTTON_A,1);SDL_JoystickUpdate();
        check(c.input(1,keyboard.data()).jump && !c.input(0,keyboard.data()).jump,"Standard cross/A jump and P2 isolation");
        SDL_JoystickSetVirtualButton(mapped,SDL_CONTROLLER_BUTTON_A,0);
        SDL_JoystickSetVirtualAxis(mapped,SDL_CONTROLLER_AXIS_LEFTX,-30000);SDL_JoystickUpdate();check(c.input(1,keyboard.data()).left,"Standard left stick");
        SDL_JoystickSetVirtualAxis(mapped,SDL_CONTROLLER_AXIS_LEFTX,0);SDL_JoystickUpdate();
        game.settings.player=1;game.settings.row=3+controls::Magic;game.settings.padColumn=true;game.settings.beginCapture(c);
        event={};event.type=SDL_CONTROLLERAXISMOTION;event.caxis.which=mappedID;event.caxis.axis=SDL_CONTROLLER_AXIS_TRIGGERRIGHT;event.caxis.value=30000;
        game.settings.captureEvent(c,event);check(!game.settings.capturing,"Capture standardized trigger");
        SDL_JoystickSetVirtualAxis(mapped,SDL_CONTROLLER_AXIS_TRIGGERRIGHT,32767);SDL_JoystickUpdate();check(c.input(1,keyboard.data()).magic,"Trigger drives mapped action");
        SDL_JoystickSetVirtualAxis(mapped,SDL_CONTROLLER_AXIS_TRIGGERRIGHT,-32768);SDL_JoystickUpdate();
        c.profile(1)->deadzone=14000;check(c.save(),"Settings save succeeds");
        controls::System loaded;loaded.path=testPath;check(loaded.load(),"Settings survive reload");
        check(loaded.keys[0][controls::Punch]==SDL_SCANCODE_F,"Keyboard persistence");
        check(loaded.profiles.at(c.profileKey(1,*c.device(1))).deadzone==14000,"Per-device deadzone persistence");
        check(loaded.profiles.at(c.profileKey(0,*c.device(0))).bind[controls::Kick].index==6,"Generic remap persistence");
        {std::ofstream out(testPath);out<<"PF_CONTROLS 1\nKEY 0 0 999999\n";}
        check(!loaded.load() && loaded.keys[0][controls::Punch]==SDL_SCANCODE_F,"Corrupt file rejects invalid indices without partial mutation");
        // Pause uses edges; a held Start does not toggle every frame.
        game.settings.open=false;game.menu=false;game.local=true;game.paused=false;game.focused=true;
        game.events();SDL_JoystickSetVirtualButton(mapped,SDL_CONTROLLER_BUTTON_START,1);SDL_JoystickUpdate();game.events();
        check(game.paused,"Start pauses match");game.events();check(game.paused,"Held Start only toggles once");
        SDL_JoystickSetVirtualButton(mapped,SDL_CONTROLLER_BUTTON_START,0);SDL_JoystickUpdate();game.events();
        SDL_JoystickSetVirtualButton(mapped,SDL_CONTROLLER_BUTTON_START,1);SDL_JoystickUpdate();game.events();check(!game.paused,"Second Start resumes");
        SDL_JoystickSetVirtualButton(mapped,SDL_CONTROLLER_BUTTON_START,0);SDL_JoystickUpdate();game.events();
        // Unplug while a button is held, then reconnect the same GUID.
        SDL_JoystickSetVirtualButton(raw,2,1);SDL_JoystickUpdate();SDL_JoystickClose(raw);raw=nullptr;
        SDL_JoystickDetachVirtual(rawIndex);game.events();check(game.paused && c.assigned[0]==-1,"Disconnect pauses and unassigns player");
        check(!c.input(0,keyboard.data()).punch,"No stuck inputs after disconnect");
        desc={};desc.version=SDL_VIRTUAL_JOYSTICK_DESC_VERSION;desc.type=SDL_JOYSTICK_TYPE_UNKNOWN;desc.naxes=4;desc.nbuttons=12;desc.nhats=1;desc.name="PF Virtual Generic";
        rawIndex=SDL_JoystickAttachVirtualEx(&desc);raw=SDL_JoystickOpen(rawIndex);game.events();
        check(c.device(0) && c.profile(0)->bind[controls::Kick].index==6,"Reconnect restores player and mapping");
        // Device assignment swaps ownership instead of allowing one pad to drive both players.
        c.cycleDevice(0,-1);check(c.assigned[0]!=c.assigned[1],"Assignments stay exclusive");
        game.settings.open=false;game.menu=true;game.focused=true;
        auto keyEvent=[&](SDL_Keycode key){SDL_Event e{};e.type=SDL_KEYDOWN;e.key.keysym.sym=key;e.key.keysym.scancode=SDL_GetScancodeFromKey(key);SDL_PushEvent(&e);game.events();};
        keyEvent(SDLK_F1);check(game.settings.open,"F1 opens actual settings menu");
        game.settings.player=0;game.settings.row=3+controls::Punch;game.settings.padColumn=false;
        keyEvent(SDLK_RETURN);check(game.settings.capturing,"Enter starts keyboard capture");
        keyEvent(SDLK_h);check(!game.settings.capturing && c.keys[0][controls::Punch]==SDL_SCANCODE_H,"Settings key event remaps action");
        game.settings.row=Settings::saveRow;keyEvent(SDLK_RETURN);check(!game.settings.open,"Save and return closes settings");
        controls::System uiSaved;uiSaved.path=testPath;check(uiSaved.load() && uiSaved.keys[0][controls::Punch]==SDL_SCANCODE_H,"UI changes persist on disk");
        game.settings.open=true;game.settings.player=0;game.settings.row=7;game.settings.padColumn=true;
        game.draw();check(game.capture("settings-virtual.bmp"),"Render settings with two controllers");
        c.close();
        if(raw){SDL_JoystickClose(raw);raw=nullptr;}
        if(mapped){SDL_JoystickClose(mapped);mapped=nullptr;}
        for(int i=SDL_NumJoysticks()-1;i>=0;i--)if(SDL_JoystickIsVirtual(i))SDL_JoystickDetachVirtual(i);
        // Parsec commonly presents a standardized Xbox 360-shaped virtual device.
        game.settings.open=false;game.menu=true;c.preferred[0]="-";c.preferred[1]="obsolete-guest-guid";
        desc={};desc.version=SDL_VIRTUAL_JOYSTICK_DESC_VERSION;desc.type=SDL_JOYSTICK_TYPE_GAMECONTROLLER;
        desc.naxes=6;desc.nbuttons=SDL_CONTROLLER_BUTTON_MAX;desc.name="PF Virtual Xbox 360 / Parsec path";desc.vendor_id=0x045e;desc.product_id=0x028e;
        int xboxIndex=SDL_JoystickAttachVirtualEx(&desc);mapped=SDL_JoystickOpen(xboxIndex);check(mapped!=nullptr,"Attach Xbox virtual controller");
        game.events();check(c.device(1) && c.device(1)->controller && !c.device(0),"New guest controller replaces stale preference while retaining keyboard P1");
        SDL_JoystickSetVirtualButton(mapped,SDL_CONTROLLER_BUTTON_X,1);SDL_JoystickUpdate();check(c.input(1,keyboard.data()).punch,"Xbox/Parsec standardized action");
        game.menuChoice=1;game.menuAccept();game.draw();check(game.capture("lobby-virtual.bmp"),"Render guest P2 lobby");
        SDL_JoystickSetVirtualButton(mapped,SDL_CONTROLLER_BUTTON_X,0);SDL_JoystickUpdate();
        c.close();SDL_JoystickClose(mapped);mapped=nullptr;SDL_JoystickDetachVirtual(xboxIndex);
        std::remove(testPath.c_str());
        std::cout<<"PASS: generic/PlayStation/Xbox virtual inputs, 8BitDo database, explicit 2P, keyboard+guest P2, stale preference recovery, hats/sticks/inverted triggers, remap/save/load, pause, reconnect\n";
        return 0;
    }catch(const std::exception&e) {
        if(raw)SDL_JoystickClose(raw);
        if(mapped)SDL_JoystickClose(mapped);
        std::remove(testPath.c_str());
        std::cerr<<"INPUT FAIL: "<<e.what()<<" / "<<SDL_GetError()<<"\n";return 1;
    }
}
