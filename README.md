# OVIMSpaceChewing for OV 0.9.1

This is branched from [OpenVanilla](https://github.com/openvanilla/openvanilla) just before [OVIMSpaceChewing was removed]
(https://github.com/openvanilla/openvanilla/commit/f769512f58bd20b7eb9e9b23b1ee293ce81a197f). And modified for libchewing master.

## How to build
1. You may need to change SDK path like [this change](https://github.com/chewing/openvanilla-chewing/commit/3d095d8fe067456994f04a2ec0cc18dee2895cbe)
2. Build libchewing
  - `-arch i386`: because old OV 0.9.1 binary only support `ppc` and `i386` architecture, force i386 arch (otherwise x86_64).
  - `--without-sqlite3`: (optional) in order to support existing uhash file.
  
 ```
 $ cd $path_to_libchewing
 $ CC='cc -arch i386 -DNDEBUG' ./configure --without-sqlite3 MAKEINFO=/usr/local/Cellar/texinfo/6.3/bin/makeinfo
 $ make all
 $ sudo make install
 ```

3. Build OVIMSpaceChewing

 ```
 $ cd $path_to_checkout_of_this_repo
 $ cd Modules/OVIMSpaceChewing
 $ make
 ```

## How to install
1. Remove 1.0 or newer if you have installed.
2. Install OpenVanilla 0.9.1.
3. Install OVIMSpaceChewing module

 ```
 $ sudo -s
 # mkdir -p /Library/OpenVanilla/0.9-Old/Modules
 # cd /Library/OpenVanilla/0.9-Old/Modules
 # unzip ~/Downloads/OVIMSpaceChewing.bundle.zip
 ```

4. Re-login

You could see [this blog article](http://ephrain.pixnet.net/blog/post/62635034) for detail instructions.

## How to install development version
1. Follow 'How to install' first.
2. Copy libchewing files into bundle.

 ```
 # cp /usr/local/lib/libchewing.3.dylib /Library/OpenVanilla/0.9-Old/Modules/OVIMSpaceChewing.bundle/Contents/Frameworks/libchewing.3.dylib
 # cp /usr/local/share/libchewing/* /Library/OpenVanilla/0.9-Old/Modules/OVIMSpaceChewing.bundle/Contents/Resources
 ```

3. Copy OVIMSpaceChewing into bundle.

 ```
 # cp $path_to_checkout_of_this_repo/Modules/OVIMSpaceChewing/OVIMSpaceChewing.dylib /Library/OpenVanilla/0.9-Old/Modules/OVIMSpaceChewing.bundle/Contents/MacOS/OVIMSpaceChewing
 ```

4. install_name_tool. It's okay to skip this step, but OVIMSpaceChewing will use libchewing from /usr/local/lib.

 ```
 # install_name_tool -change /usr/local/lib/libchewing.3.dylib @loader_path/../Frameworks/libchewing.3.dylib /Library/OpenVanilla/0.9-Old/Modules/OVIMSpaceChewing.bundle/Contents/MacOS/OVIMSpaceChewing
 ```

5. Re-login

## How to pack the bundle (for release)
```
# cd /Library/OpenVanilla/0.9-Old/Modules
# touch OVIMSpaceChewing.bundle
# zip -r OVIMSpaceChewing.bundle.zip OVIMSpaceChewing.bundle
```

## License
As original OVIMSpaceChewing, Artistic License (for Modules/OVIMSpaceChewing folder).
