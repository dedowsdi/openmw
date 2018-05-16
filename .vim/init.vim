call myvim#switchRtp('./.vim')
let g:mycppBuildDir = './build/clang/RelWithDebInfo/'

au BufNewFile,BufRead * 
      \ if &ft == '' && match(expand('%:p'), 'OpenSceneGraph/include\|include/osg') != -1   | set ft=cpp | endif

nnoremap \tq A// @Ques<esc>

let g:clang_format#style_options = {
      \ 'AccessModifierOffset' : -4,
      \ 'AllowShortFunctionsOnASingleLine' : 'true',
      \ 'AllowShortIfStatementsOnASingleLine' : 'true',
      \ 'AllowShortLoopsOnASingleLine' : 'true',
      \ 'AlwaysBreakTemplateDeclarations' : 'true',
      \ 'AlignAfterOpenBracket' : 'false',
      \ 'ContinuationIndentWidth' : 4,
      \ 'IndentWidth' : 4,
      \ 'TabWidth' : 4,
      \ 'UseTab' : 'Never',
      \ 'Standard' : 'C++11',
      \ 'SortIncludes': 'false',
      \ 'ColumnLimit': 120,
      \ 'BreakBeforeBraces': 'Custom',
      \ 'BraceWrapping' : {
      \     'AfterClass'            : 'true',
      \     'AfterControlStatement' : 'true',
      \     'AfterEnum'             : 'true',
      \     'AfterFunction'         : 'true',
      \     'AfterNamespace'        : 'true',
      \     'AfterObjCDeclaration'  : 'true',
      \     'AfterStruct'           : 'true',
      \     'AfterUnion'            : 'true',
      \     'BeforeCatch'           : 'true',
      \     'BeforeElse'            : 'true',
      \     'IndentBraces'          : 'false',
      \ }
      \}

" use whatever tool to generate quickfix list, copy literal content of quickfix
" list above record.cpp, just above the addRecordProperty macro, copy newly
" wanted record to register r, then call this function
function! AddProperty()
  let abbreMap={
  \ 'acti': 'Activator',
  \ 'alch': 'Potion',
  \ 'armo': 'Armor',
  \ 'appa': 'Apparatus',
  \ 'body': 'BodyPart',
  \ 'book': 'Book',
  \ 'bsgn': 'BirthSign',
  \ 'cell': 'Cell',
  \ 'clas': 'Class',
  \ 'clot': 'Clothing',
  \ 'cont': 'Container',
  \ 'crea': 'Creature',
  \ 'dial': 'Dialogue',
  \ 'door': 'Door',
  \ 'ench': 'Enchantment',
  \ 'fact': 'Faction',
  \ 'glob': 'Global',
  \ 'gmst': 'GameSetting',
  \ 'info': 'DialInfo',
  \ 'ingr': 'Ingredient',
  \ 'land': 'Land',
  \ 'levlist': 'CreatureLevList',
  \ 'levi': 'ItemLevList',
  \ 'ligh': 'Light',
  \ 'lock': 'Lockpick',
  \ 'prob': 'Probe',
  \ 'repa': 'Repair',
  \ 'ltex': 'LandTexture',
  \ 'mgef': 'MagicEffect',
  \ 'misc': 'Miscellaneous',
  \ 'npc': 'NPC',
  \ 'pgrd': 'Pathgrid',
  \ 'race': 'Race',
  \ 'regn': 'Region',
  \ 'scpt': 'Script',
  \ 'skil': 'Skill',
  \ 'sndg': 'SoundGenerator',
  \ 'soun': 'Sound',
  \ 'spel': 'Spell',
  \ 'sscr': 'StartScript',
  \ 'stat': 'Static',
  \ 'weap': 'Weapon',
  \ }   

	let lnum = line('.')
  while getline(lnum) =~? '\vcomponents\/esm\/load.*'
    let abbr = matchstr(getline(lnum), '\vcomponents\/esm\/load\zs\w+\ze\.')
    if !has_key(abbreMap, abbr)
      echoe 'failed to find class for ' . abbr
      break
    endif

    let className = abbreMap[abbr]
    call cursor(lnum, 1)
    if search(printf('\VESM::%s', className), 'W')
      call search('{')
      normal! %
      normal! "rP
      echom "add property for " . className
    else
      echoe 'faled to search class ' . className
    endif

    let lnum+=1
  endwhile
      
endfunction
