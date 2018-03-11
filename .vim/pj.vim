"debugger, lldb or gdb
set rtp +=./.vim
set rtp +=./.vim/after
"build dir, such as ./build/Debug, ./build/RelWithDebInfo
let g:mycppBuildDir = "./build/clang/Debug/"

" abbrivation
au BufNewFile,BufRead * 
      \ if &ft == '' && match(expand('%:p'), 'OpenSceneGraph/include\|include/osg') != -1   | set ft=cpp | endif

nnoremap \tq A// @Ques<esc>
