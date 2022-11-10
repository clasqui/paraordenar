#!/bin/bash

function _pro_completions()
{
    return_completions=$(pro -r _completion $COMP_CWORD <<< "${COMP_WORDS[@]:1}")
    completion_list=$(echo "${return_completions}" | awk -F':' '{print $1}')
    COMPREPLY=($(compgen -W "${completion_list}" "${COMP_WORDS[${COMP_CWORD}]}"))
}

complete -F _pro_completions pro
