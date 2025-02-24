@SET PATH=%PATH%;c:\programs\Graphviz2.38\bin

@rem dot -Tps gr1.gv -o gr1.ps
@rem dot -Tps %1 -o %1.ps

del *.gif

@for %%f in (*.gv) do dot -Tgif %%f -o %%f.gif
