#!/bin/bash

element_index()
{
  match=$1
  shift

  index=0
  for element in "$@"; do
    [[ "$element" == "$match" ]] && echo $index && return 0
    index=$(( index + 1))
  done

  echo -1

  return 1
}

element_in()
{
  index=$(element_index "$@")
  [[ $index -eq -1 ]] && return 1
  return 0
}

_generate_bt_candidates()
{
  COMPREPLY=()
  quiet=--quiet

  # use default for file name
  reOption='^-'
  reFile='^--file$|^-f$'
  if [[ ( $COMP_CWORD -eq 1 && ! $2 =~ $reOption ) || $3 =~ $reFile ]]; then
    compopt -o bashdefault
    compopt -o default
    return 0
  fi

  compopt +o bashdefault
  compopt +o default

  # option
  case "$2" in
    -*) COMPREPLY=( $( compgen -W '
                          --help
                          --version
                          --quiet
                          --file
                          --name
                          --regex
                          --' -- "$2" ) )
        return 0 ;;
  esac

  local file candidates cur options
  candidates=()
  cur="${2//\\\\/\\}"
  cur="${cur//\\\\/\\}"
  cur="${cur//\\\\/\\}"
  cur="${cur//\\/\\\\\\\\}"
  cur="${cur//\'/\\\'}"

  options=($quiet)

  case $3 in
    --name )

      # no completion for regex style
      element_in "--regex" "${COMP_WORDS[@]}" && return 0

      name=$2

      [[ -n "$file" ]] && options+=(--file $file)
      [[ -n "$name" ]] && options+=(--regex --name "^$id.*")

      ;;

  esac

  if [[ ${#options[@]} -gt 0 ]]; then
      mapfile -t candidates < <( bt "${options[@]}" )
  fi

  if [[ ${#candidates[@]} -gt 0 ]]; then
      candidates=( "${candidates[@]//\\/\\\\\\\\}" )
      candidates=( "${candidates[@]//\'/\\\'}" )
      mapfile -t COMPREPLY< <( IFS=$'\n'; compgen -W "${candidates[*]}" -- "$cur")
      return 0
  fi

  return 0
}

complete -F _generate_bt_candidates bsa_viewer
complete -F _generate_bt_candidates bt
