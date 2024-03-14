require "second_file" 

function OnAttach()
    log.info("TwoFiles::OnAttach()")
    second_file_func()
end
