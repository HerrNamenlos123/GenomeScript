require "second_file" 

function OnAttach()
    log.info("TwoFiles::OnAttach()")
    second_file_func()
end

function eCVisualAnimation_PS_PlayMotion(this, type, motion)
    print("PlayMotion: " .. this:GetResourceFilePath():GetText())
end

function eCWrapper_emfx2Motion_LoadMotion(this, file)
    print("LoadMotion")
end