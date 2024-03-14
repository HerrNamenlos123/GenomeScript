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
            args.AniSpeedScale = args.AniSpeedScale * 3.5
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

    log.warn("Changed pacing of combat motion")
    PreventDefaultWithValue(gCScriptProcessingUnit.sAICombatMoveInstr(reinterpret_voidptr(args), spu, fullstop))
end

function eCVisualAnimation_PS_PlayMotion(this, type, motion)
    log.info("PlayMotion: " .. this:GetResourceFilePath():GetText())
end

function eCWrapper_emfx2Motion_LoadMotion(this, file)
    print("LoadMotion")
end