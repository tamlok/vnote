# VNote
[中文 Chinese](./README_zh.md)

[VNote Homepage](https://tamlok.github.io/vnote)

[Documentation](http://vnote.readthedocs.io/en/latest/)

[Screenshots](https://tamlok.github.io/vnote/gallery.html)

Designed specially for **Markdown**, **VNote** is a Vim-inspired note-taking application, which knows programmers and Markdown better.

![VNote](screenshots/vnote.png)

# Downloads
Users from China can download the latest release of VNote from [Baidu Netdisk](http://pan.baidu.com/s/1jI5HROq).

## Windows
![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/tamlok/vnote?svg=true)

- [Github releases](https://github.com/tamlok/vnote/releases)
- Latest builds on master: [ ![Download](https://api.bintray.com/packages/tamlok/vnote/vnote/images/download.svg) ](https://bintray.com/tamlok/vnote/vnote/_latestVersion)

**NOT** supported in XP since QtWebEngineProcess used by VNote could not work in XP.

## Linux

### AppImage
[![Build Status](https://travis-ci.org/tamlok/vnote.svg?branch=master)](https://travis-ci.org/tamlok/vnote)

There is an AppImage format standalone executable of VNote for major Linux distributions. **Any help for packaging and distribution on Linux is appreciated!**

- [Github releases](https://github.com/tamlok/vnote/releases)
- Latest builds on master: [ ![Download](https://api.bintray.com/packages/tamlok/vnote/vnote/images/download.svg) ](https://bintray.com/tamlok/vnote/vnote/_latestVersion)

### openSUSE
Currently `vnote` on openSUSE Tumbleweed can be installed from `home:opensuse_zh` project on OBS. You can execute the following command directly:

```shell
sudo zypper ar https://download.opensuse.org/repositories/home:/opensuse_zh/openSUSE_Tumbleweed/ home:opensuse_zh
sudo zypper ref
sudo zypper in vnote
```

For other architectures, please search for `vnote` at [software.opensuse.org](https://software.opensuse.org).

We don't support Leap 42 and below due to the Qt version. Please use AppImage or build it yourself.

### Arch Linux
VNote on Arch Linux can be installed from the AUR as [vnote-git](https://aur.archlinux.org/packages/vnote-git/):

```shell
git clone https://aur.archlinux.org/vnote-git.git
cd vnote-git
makepkg -sic
```

## MacOS
[![Build Status](https://travis-ci.org/tamlok/vnote.svg?branch=master)](https://travis-ci.org/tamlok/vnote)

- [Github releases](https://github.com/tamlok/vnote/releases)
- Latest builds on master: [ ![Download](https://api.bintray.com/packages/tamlok/vnote/vnote/images/download.svg) ](https://bintray.com/tamlok/vnote/vnote/_latestVersion)

You can also install VNote using homebrew, through the cask tap:

```shell
brew cask install vnote
```

# Description
**VNote** is a Qt-based, free and open source note-taking application, focusing on Markdown. VNote is designed to provide comfortable edit experience.

VNote is **NOT** just a simple editor for Markdown. By providing notes management, VNote makes taking notes in Markdown simpler and more pleasant.

Utilizing Qt, VNote could run on **Linux**, **Windows**, and **macOS**.

![VNote Main Interface](screenshots/_vnotemaini_1521763751_1643978501.png)

# Supports
- [Github issues](https://github.com/tamlok/vnote/issues);
- Email: tamlokveer at gmail.com;
- WeChat Public Account: VNote笔记 (vnote_md);  
![VNote WeChat](screenshots/vnote_md.jpg)

# Highlights
- Powerful **full-text search**;
- **Universal Entry** to reach anything just by typing;
- Inserting images directly from clipboard;
- Syntax highlights of fenced code blocks in both **edit** and **read** mode;
- Outline in both edit and read mode;
- Custom styles in both edit and read mode;
- Vim mode and a set of powerful shortcuts;
- Infinite levels of folders;
- Multiple tabs and splitting windows;
- [Mermaid](http://knsv.github.io/mermaid/), [Flowchart.js](http://flowchart.js.org/), and [MathJax](https://www.mathjax.org/);
- Supports HiDPI;
- Attachments of notes;
- Themes and dark mode;
- Rich and extensible export, such as HTML, PDF, and PDF (All In One);

# Donate
You could help VNote's development in many ways.

- Keep monitoring VNote and sending feedback for improvement.
- Spread and promote VNote to your friends. Popularity is a strong power to drive developers.
- Participate in the development of VNote and send PullRequest to make VNote perfect.
- Last, really appreciate your donate to VNote if you think VNote really helps you and you want to help VNote.
    - PayPal: [PayPal.Me/vnotemd](https://www.paypal.me/vnotemd)
    - Alipay: `tamlokveer@gmail.com`  
    ![Alipay](screenshots/alipay.png)

Thanks very much to [them](https://github.com/tamlok/vnote/wiki/Donate-List) who donated to VNote!

# Why VNote
## Markdown Editor & Notes Management
VNote tries to be a powerful Markdown editor with notes management, or a note-taking application with pleasant Markdown support. If you are a fan of Markdown and enjoy writing Markdown notes for study, work, and life, VNote is the right tool for you.

## Pleasant Markdown Experience
### Insights About Markdown
Markdown, as a simple mark language, unlike rich text, was born with a **gap between edit and read**. There are about three ways to handle this gap:

1. As one extreme, some editors just treat Markdown as **plain text**. Users may lose themselves in the messy black characters. It is hard to keep track of the information of the note.
2. Most Markdown editors use two panels to **edit and preview Markdown notes simultaneously**. It makes things easier since users could see a pleasant typesetting and layout while editing the text. However, two panels may occupy the whole screen and users keep moving eyes left and right which will be a big distraction.
3. As another extreme, some editors convert the Markdown elements in place immediately after the typing, which makes editing Markdown just like editing rich text document in Word.

Since most editors choose the second way to handle the gap, people always think of preview when it comes to Markdown. It may be a misunderstanding about Markdown. Designed as a simple mark language, Markdown is intended to help to keep track of the information of the text when editing and provide a beautiful typesetting when reading after being converted to HTML.

### Tradeoff: VNote's Way
VNote tries to minimize the gap and provide the best-effort *WYSIWYG* for Markdown via tuned **syntax highlights** and some other features. By helping to keep track of the content, there is no need to preview or alter the text immediately after being typed.

# Features
## Notebook-Based Notes Management
VNote uses **notebooks** to hold your notes. Like OneNote, a notebook can be hosted on any location on your system. A notebook is designed to represent one account. For example, you could have one notebook hosted on local file system and another notebook hosted on an OwnCloud server. This is really useful when notes require different levels of security.

A notebook corresponds to a self-contained folder (called the *Root Folder* of the notebook) in the file system. You could copy the folder to another location (or on another computer) and import it into VNote.

A notebook could have infinite levels of folders. VNote supports copying or moving folders or notes within or between notebooks.

## Simple Notes Management
All your notes are managed by some plaintext configuration files and stored as plaintext files. You could access your notes without VNote. You could use external file synchronization services to synchronize your notes and import them on another machine.

VNote supports both Markdown (suffix `md`) and rich text notes.

## Syntax Highlight
VNote supports accurate syntax highlight for Markdown. Tuning the style of the highlight, VNote enables you to keep track of your document easily.

VNote also supports **syntax highlight for fenced code blocks** in edit mode in Markdown, which is **superior** than almost all current Markdown editors.

![Syntax Highlight](screenshots/_1513485266_1616037517.png)

## Live Image Preview
VNote supports previewing the image link in edit mode. With this, you could just stay in edit mode as much as possible.

![Live Image Preview](screenshots/_1513485934_140085443.png)

## Pleasant Image Experience
Just paste your image into the Markdown note, VNote will manage all other stuffs. VNote stores images in a specified folder in the same folder with the note. VNote will pop up a window to preview the image when you insert an image. Furthermore, VNote will delete the useless image files automatically after you remove the image links.

![](screenshots/_1513485753_394180887.png)

## Interactive Outline Viewer In Read & Edit Mode
VNote provides a user-friendly outline viewer for both edit and view mode. The outline viewer is a responsive item tree instead of a segment of HTML.

## Powerful Shortcuts
VNote supports many pleasant and powerful shortcuts which facilitate your editing, including **Vim Mode**, **Captain Mode**, and **Navigation Mode** which enable you to work without the mouse.

Please refer to the [shortcuts help](src/resources/docs/shortcuts_en.md) in the help menu for more details.

## Highly Configurable
In VNote, almost everything is configurable, such as background color, font, and Markdown style. VNote uses a plaintext file to record all your configuration, so you could just copy that file to initialize a new VNote on another computer.

# Dependencies
- [Qt 5.9](http://qt-project.org) (L-GPL v3)
- [PEG Markdown Highlight](http://hasseg.org/peg-markdown-highlight/) (MIT License)
- [Hoedown 3.0.7](https://github.com/hoedown/hoedown/) (ISC License)
- [Marked](https://github.com/chjj/marked) (MIT License)
- [Highlight.js](https://github.com/isagalaev/highlight.js/) (BSD License)
- [Ionicons 2.0.1](https://github.com/driftyco/ionicons/) (MIT License)
- [markdown-it 8.3.1](https://github.com/markdown-it/markdown-it) (MIT License)
- [markdown-it-headinganchor 1.3.0](https://github.com/adam-p/markdown-it-headinganchor) (MIT License)
- [markdown-it-task-lists 1.4.0](https://github.com/revin/markdown-it-task-lists) (ISC License)
- [mermaid 7.0.0](https://github.com/knsv/mermaid) (MIT License)
- [MathJax](https://www.mathjax.org/) (Apache-2.0)
- [showdown](https://github.com/showdownjs/showdown) (Unknown)
- [flowchart.js](https://github.com/adrai/flowchart.js) (MIT License)

# License
VNote is licensed under the [MIT license](http://opensource.org/licenses/MIT).
