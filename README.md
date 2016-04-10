# Android_pstree

Implementation of process tree by dfs in Android virtual machine. 

Instructions:
1.Add a new system call in android. Type $emulator -avd #Android vm name# -kernel #your kernel location# in terminal. Put sys_mycall.c and Makefile to a new folder and type $make in it. Push *.ko file to Android vm.

2.Use this new system call. Put ptree.c and Android.mk to a new folder and type $ndk-build(install android ndk and sdk first) in it. Push the ptree file in /obj to Android vm. Type ./ptree, then it will print the process tree.
