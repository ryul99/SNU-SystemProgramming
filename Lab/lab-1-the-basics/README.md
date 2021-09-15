# Lab 1 - The Basics

## CSAP VM
1. VirtualBox -> Import
2. Settings -> Shared Folders -> Add  
     Folder Path: select folder on your harddrive  
     Folder Name: devel_share **(exact match)**
3. HiDPI screens: set scaling factor  
     Settings -> Display -> Scale Factor: 200%
4. Take snapshot
5. Start VM and login  
     Credentials: devel / devel
6. Shutdown / Revert to snapshot

## Git Setup

Set global identity
```bash
git config --global user.name "Bernhard Egger"
git config --global user.email bernhard@csap.snu.ac.kr
```

You can cache your password for a given amount of seconds so you won't have to enter it every single time
```bash
git config --global credential.helper 'cache --timeout=3600'
```

Alternatively, you can use the `store` credential helper - this will store you password in clear-text and once and for all
```bash
git config --global credential.helper 'store'
```
Obviously, this is a very bad idea on a public/shared machine.


## Vim Setup
Real programmers use vim

### vim-fugitive plugin
Shows the Git status in vim  
https://github.com/tpope/vim-fugitive

Installation
```bash
mkdir -p ~/.vim/pack/tpope/start
cd ~/.vim/pack/tpope/start
git clone https://tpope.io/vim/fugitive.git
vim -u NONE -c "helptags fugitive/doc" -c q
```

Edit ~/.vimrc to contain the following
```viml
"-------------------------------------------------------------------------------
" Fugitive plugin: show Git status in statusline
"
hi CVSStatus cterm=bold ctermbg=LightGray ctermfg=DarkBlue
hi StatusLine cterm=NONE ctermbg=LightGray ctermfg=Black
set laststatus=2
set statusline=
set statusline+=%#CVSStatus#%{FugitiveStatusline()}
set statusline+=%#StatusLine#\ %<%f\ %h%m%r%=%-14.(%l,%c%V%)\ %P
```

### tagbar plugin
Easier navigation in vim  
https://github.com/majutsushi/tagbar

Installation
```bash
mkdir -p ~/.vim/pack/majutsushi/start
cd ~/.vim/pack/majutsushi/start
git clone https://github.com/majutsushi/tagbar.git
vim -u NONE -c "helptags tagbar/doc" -c q
```

Edit ~/.vimrc to contain the following
```viml
"-------------------------------------------------------------------------------  
" tagbar plugin configuration
"
let g:tagbar_width=52
autocmd VimEnter * nested :call tagbar#autoopen(1)
autocmd FileType * nested :call tagbar#autoopen(0)
autocmd BufEnter * nested :call tagbar#autoopen(0)
```
Modify `g:tagbar_width ` as you please.

Delete/comment out all Tlist related options:
```viml
"-------------------------------------------------------------------------------
" Tlist plugin configuration
"
" disabled in favor of tagbar
"let Tlist_Auto_Open = 1
"let Tlist_Auto_Update = 1
""autocmd FileType c,cpp,h TlistOpen
"let Tlist_Compact_Format = 1
"let Tlist_Exit_OnlyWindow = 1
"let Tlist_GainFocus_On_ToggleOpen = 0
"let Tlist_WinWidth = 29
```

### YouCompleteMe plugin
Advanced IDE features for vim  
https://github.com/ycm-core/YouCompleteMe

Installation
```bash
mkdir -p ~/.vim/bundle
cd ~/.vim/bundle
git clone https://github.com/ycm-core/YouCompleteMe.git
cd YouCompleteMe
git submodule update --init --recursive
python3 install.py --clangd-completer
```

Edit ~/.vimrc to contain the following
```viml
"-------------------------------------------------------------------------------
" youcompleteme plugin configuration
"
set rtp+=~/.vim/bundle/YouCompleteMe
autocmd BufRead,BufNewFile * setlocal signcolumn=yes
```
