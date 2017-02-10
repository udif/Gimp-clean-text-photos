# Fix Text Background plugin for GIMP

This plugin is used to clean text scans that were taken with a camera, not a scanner.
Typical text scans of this kind suffers from uneven lighting and curled pages that cause more light reflections.

## How the plugin works
The plugin starts by turning the selected area into a geryscale image using the same formula as Colors/Desaturate/Luminosity tool.
The plugin then works by dividing the photo into small blocks, and process each block inddependently.
Theer are two parameters defining those blocks.
The kernel size defines the square radius that is looked at on each block (each block is (2*kernel_size+1)^2 )
The inner_size is that square radius ((2*inner_size+1)^2 )of the area that is actually modified.
The area covered by the kernel is measured (histogram) and is used to update the inner_size area.
The inner_size is always centered within the kenel_size.
After each square block is done, we advance by (2*inner_size+1).

## What does the plugin do on each block?
Basically, without going into details, it tries to separate foreground from background pixels,
and clearing (setting to a white color) the backgroiund pixels.

## And specifically, how is that done?
A histogram is built on each kernel, and the peak is found.
We assume this peak is background, and find the first local minima in the histogram darker than this peak.
We then determine if this peak is due to foreground or background data,
by comparing thepek location to the average value of the kernel.
If we find that the peak is lower than the average pixel value, we choose the average value as threshold instead.
We then clear to white every pixel larger than the threshold.

# TODO
The algorithm works well on 95% of the images I wrote it for, but fails on large foreground areas
I have a few more ideas to tweak it.
Also need to add  a GUI to control such parameter as kernel_size and inner_size.

Useful links:
[How to configure gtk on Visual studio 2010](http://stackoverflow.com/questions/15906580/how-to-configure-gtk-on-visual-studio-2010)
[MakeDef : Intelligent DEF file Generator for Win32 Apps](http://www.codeguru.com/tools/standalonetools/article.php/c1351/MakeDef--Intelligent-DEF-file-Generator-for-Win32-Apps.htm)
[How to build a GIMP plug-in with Visual Studio](http://web.archive.org/web/20150906090521/http://registry.gimp.org/node/24883)
[Visual Studio 2015 Compiler options](https://msdn.microsoft.com/en-us/library/9s7c9wdw.aspx)
[Writing a GIMP Plug-In](https://www.gimp.org/docs/plug-in/plug-in.html)
[How to write a GIMP Plug-In](https://developer.gimp.org/writing-a-plug-in/1/index.html)
[GIMP Scripts and Plug-ins](http://gimpbook.com/scripting/slides30/)
