
function Music_TriggerFight(this)
    PreventDefaultAsSuccess()
    log.warn("Prevented trigger for combat music")
end

-- When doing a revolution in a city, there is still combat music.
-- function Music_TriggerRevolution(this)
--     PreventDefaultAsSuccess()
--     log.warn("Prevented trigger for revolution music")
-- end