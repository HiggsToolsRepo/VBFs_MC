INTRODUCTION 
-------------

In git every repository is a full copy. Then there is the github copy,
which is the one we share.

The basic workflow to work in your *own* repository is:

* Select the files you are going to commit:
```bash
git add <file1> <file2> ...
```
To add everything:
```
git add .
```
You can ignore some changes when running this command by adding the corresponfing filenames to a .gitignore file (also works for folders and extensions ie '*.aux', see examples online).

* When you have everything added do:
```
git commit
```
and write a DESCRIPTIVE message of what have you done. This is useful
for your collaborators as well as your future self.

You can look at what your repository looks like with
```
git status
```
however a much nicer way is to add the following to your .bashrc

```bash
export GIT_PS1_SHOWDIRTYSTATE=1
export GIT_PS1_SHOWUNTRACKEDFILES=true
export PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[01;34m\] \w\[\033[00;33m\]$(__git_ps1)\[\033[01;34m\] \$\[\033[00m\] '
```

The last line is linuxmint-specific (because of the colors). Search git ps1 <yout os> for a
better alternative.

Yu can see the past commits with

```
git log
```

BELOW IS MORE INFO THAN PROBABLY NEEDED AND LESS THAN PROBABLY USEFUL
---------------------------------------------------------------------

Adding experimental features

When changing thinks that can possibly break something, it is better
to use branches.

you do something like
```
git checkout -b <mynewexperimentalfeature>
```
This creates a new branch and points you to it. You can switch to
another branch, with git checkout <branch> (without the -b). 

The default branch is called "master".

In your new branch,  add your commits as above. Once it is stabilized
and working, you can merge back in the main code with:
```
git checkout master
git merge <mynewexperimentalfeature>
```

Sharing your commits (DO READ THIS)
----------------------------------

First make sure you don't have uncommited changes.

Then download the commits that you don't have:
```
git pull
```
At this point you should pray that there are no conflicts (not discussed here). 

Then push your existing changes to the github server:
```
git push
```
