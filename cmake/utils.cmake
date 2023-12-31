function(find_most_recent_program _var)
    if (NOT DEFINED ${_var})
        unset(_found CACHE)
        execute_process(COMMAND bash -c "compgen -c ${ARGV1} | grep -E '${ARGV1}-[[:digit:]]+' | head -n1" OUTPUT_VARIABLE MOST_RECENT_PROGRAM)
        string(STRIP ${MOST_RECENT_PROGRAM} MOST_RECENT_PROGRAM)
        find_program(_found ${MOST_RECENT_PROGRAM})
        if (_found AND NOT _found IN_LIST ${_var})
            set(${_var} "${${_var}};${_found}" CACHE FILEPATH "Path to a program." FORCE)
        else()
            unset(_found CACHE)
        endif()
    endif()
endfunction()
