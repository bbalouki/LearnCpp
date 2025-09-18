cmake_minimum_required(VERSION 3.23)


function(setup_target target_name)
    message(STATUS
        "Setting C++20 standard for :${target_name}"
    )
    target_compile_features(${target_name} PUBLIC cxx_std_20)
endfunction()
