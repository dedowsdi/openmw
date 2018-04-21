call myvim#switchRtp('./.vim')
let g:mycppBuildDir = "./build/clang/Debug/"

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
