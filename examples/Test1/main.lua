function OnAttach()
    log.info("Hello world")
end

-- function eCApplication_Processs(this)
--     -- log.info("ProcessHook")
--     --Music.TriggerExplore()
--     --Music.TriggerSituation(bCString("test"))
-- end

function eCApplication_PlayVideo(this, video)
    log.info("PlayVideoHook: " .. video:GetText())
    -- PreventDefaultAsSuccess()
end

function gCScriptProcessingUnit_AISetState(this, state)
    log.info("AISetStateHook: " .. String(state))
end

function gCScriptProcessingUnit_sAICombatMoveInstr(args, spu, fullstop)
    if args ~= nil then
        if Int(args.Action) == 1 then -- Normal attack
            args.AniSpeedScale = args.AniSpeedScale * 1.5
        end
        if Int(args.Action) == 11 then -- Pierce attack
            args.AniSpeedScale = args.AniSpeedScale * 1.2
        end
        if Int(args.Action) == 2 then -- Power attack
            args.AniSpeedScale = args.AniSpeedScale * 1.3
        end
        if Int(args.Action) == 34 then -- Pierce Stumble
            args.AniSpeedScale = args.AniSpeedScale * 2
        end
        -- if Int(args.Action) == 5 then
        --     --args.Action = gEAction(4)
        --     --args.Action = gEAction(32)
        -- end
        -- if Int(args.Action) == 32 then
        --     args.Action = gEAction(27)
        -- end
        --args.AniSpeedScale = args.AniSpeedScale * 2.5
        -- log.info("AICombatMoveInstrHook: " .. Int(args.Action))
    end
    
    -- Prevent Default because we don't want to call the original after this script, instead we call
    -- the original function from here but with modified parameters. That means, instead of this script running and
    -- then the original running in C++, the script already calls the original and then tells C++ to
    -- instead of calling it again, returning early with the return value. Makes sense? :/

    PreventDefaultWithValue()
    return gCScriptProcessingUnit.sAICombatMoveInstr(reinterpret_voidptr(args), spu, fullstop)
end