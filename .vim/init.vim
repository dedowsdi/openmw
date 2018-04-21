call myvim#switchRtp('./.vim')
let g:mycppBuildDir = "./build/clang/Debug/"

au BufNewFile,BufRead * 
      \ if &ft == '' && match(expand('%:p'), 'OpenSceneGraph/include\|include/osg') != -1   | set ft=cpp | endif

nnoremap \tq A// @Ques<esc>
