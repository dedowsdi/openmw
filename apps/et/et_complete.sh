#!/bin/bash

# print index start from 0
# needle elem0 elem1 elem2....
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

# print single value of 1st element
element_value()
{
  index=$(element_index "$@")
  [[ $index -eq -1 ]] && echo "" && return 1
  value_index=$(( index + 3 )) # 0th is cmd, 1st is element
  [[ $# -lt $value_index ]] && echo "" && return 1
  echo "${!value_index}"
  return 0
}

# print multiple values of 1st option
option_value()
{
  # get option index
  option_index=$(element_index "$@")
  [[ $option_index -eq -1 ]] && echo "" && return 1
  value_index=$(( option_index + 3))
  # output value until next option
  reOption='^-'
  for i in $(seq $value_index $# ); do
    [[ ${!i} =~ $reOption ]] && return 0
    echo "${!i}"
  done

  return 0
}

current_option()
{
  reOption='^-'
  for index in $(seq -s ' ' 1 "$COMP_CWORD" | rev); do
    [[ "${COMP_WORDS[$index]}" =~ $reOption ]] && echo "${COMP_WORDS[$index]}" && return 0
  done
  return 0
}

# find first valid needle with a single valid value
# number of needles, needle0, needle1, elem0, .....
elements_value()
{
  data=( "${@:$(($1+2))}" )
  for needle in $(seq 2 "$(($1 + 1))"); do
    value=$(element_value "${!needle}" "${data[@]}")
    [[ -n $value ]] && echo "$value" && return 0
  done
  return 1
}

# find first valid needle with multiple valid value
# number of needles, needle0, needle1, elem0, .....
options_value()
{
  data=( "${@:$(($1+2))}" )
  for needle in $(seq 2 "$(($1 + 1))"); do
    value=$(option_value "${!needle}" "${data[@]}")
    [[ -n $value ]] && echo "$value" && return 0
  done
  return 1
}

_generate_et_candidates()
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
                          --file
                          --verbose
                          --cellref-verbose
                          --type
                          --cellref-type
                          --id
                          --cellref-id
                          --cell-include-id
                          --cell-include-type
                          --regex
                          --hide-type
                          --list-type
                          --list-mgef
                          --list-skil
                          --list-property
                          --list-property-name
                          --list-inventory
                          --include-property
                          --include-inventory
                          --include-inventory-count
                          --property-value
                          --include-spell
                          --' -- "$2" ) )
        return 0 ;;
  esac

  case $( current_option ) in
    --type | -t | --cellref-type | -T | --cell-include-type )

      COMPREPLY=( $( compgen -W "$(et --list-type)" -- "$2") )
      return 0
      ;;
  esac

  local file type id cell_include_type cellref_type options candidates cur include_property
  candidates=()
  cur="${2//\\\\/\\}"
  cur="${cur//\\\\/\\}"
  cur="${cur//\\\\/\\}"
  cur="${cur//\\/\\\\\\\\}"
  cur="${cur//\'/\\\'}"

  type=$(options_value 2 "--type" "-t" "${COMP_WORDS[@]}")
  cellref_type=$(options_value 1 "--cellref-type" "${COMP_WORDS[@]}")
  cell_include_type=$(options_value 2 "--cell-include-type" "-T" "${COMP_WORDS[@]}")
  include_property=$(options_value 1 "--include-property" "${COMP_WORDS[@]}")

  case $3 in
    --id | -i | --cell-include-id | --cellref-id )

      # no completion for regex style
      element_in "--regex" "${COMP_WORDS[@]}" && return 0
      element_in "-r"  "${COMP_WORDS[@]}" && return 0

      [[ ! "${COMP_WORDS[1]}" =~ $reOption ]] && file="${COMP_WORDS[1]}"
      [[ -z $file ]] && file=$(elements_value 2 "--file" "-f" "${COMP_WORDS[@]}")
      file=$(eval echo "$file")

      id=$2 # different from COMP_WORDS,  $2 won't include surrounding "" or ''

      options=(--hide-type --verbose 1 $quiet)
      [[ -n "$file" ]] && options+=(--file $file)
      [[ -n "$id" ]] && options+=(--regex --id "^$id.*")

      if [[ "$3" == '--id' || "$3" == '-i' ]]; then
        [[ -n "$type" ]] && options+=(--type $type)
      fi
      if [[ "$3" == '--cellref-id' ]]; then
        [[ -n "$cellref_type" ]] && options+=(--type $cellref_type)
      fi
      if [[ "$3" == '--cell-include-id' || "$3" == '-I' ]]; then
        [[ -n "$cell_include_type" ]] && options+=(--type $cell_include_type)
      fi

      ;;

    --include-property | --list-property )
      options=(--list-property-name)
      [[ -n "$type" ]] && options+=(--type $type)
      ;;

    --property-value )

      options=(--list-property "$include_property")
      [[ -n "$type" ]] && options+=(--type $type)
      ;;

    --include-inventory )
      options=(--list-inventory)
      [[ -n "$type" ]] && options+=(--type $type)
      ;;

    --include-spell )
      options=(--type spel --hide-type)
      ;;

  esac

  if [[ ${#options[@]} -gt 0 ]]; then
      mapfile -t candidates < <( et "${options[@]}" )
  fi

  if [[ ${#candidates[@]} -gt 0 ]]; then
      candidates=( "${candidates[@]//\\/\\\\\\\\}" )
      candidates=( "${candidates[@]//\'/\\\'}" )
      mapfile -t COMPREPLY< <( IFS=$'\n'; compgen -W "${candidates[*]}" -- "$cur")
      return 0
  fi

  return 0
}

complete -F _generate_et_candidates et
