#@category PicSay
#@menupath Tools.PicSay.Export native pseudocode
#@toolbar

import os
from ghidra.app.decompiler import DecompInterface
from ghidra.util.task import ConsoleTaskMonitor

out_dir = os.environ.get("PICSAY_DECOMP_OUT", os.path.join(os.getcwd(), "decomp"))
if not os.path.isdir(out_dir):
    os.makedirs(out_dir)

program_name = currentProgram.getName()
safe_name = program_name.replace("/", "_").replace("\\", "_")
listing = currentProgram.getListing()
fm = currentProgram.getFunctionManager()
monitor = ConsoleTaskMonitor()

meta_path = os.path.join(out_dir, safe_name + ".functions.txt")
c_path = os.path.join(out_dir, safe_name + ".decompiled.c")

decomp = DecompInterface()
decomp.openProgram(currentProgram)

functions = list(fm.getFunctions(True))
functions.sort(key=lambda f: f.getEntryPoint().getOffset())

meta = open(meta_path, "w")
code = open(c_path, "w")
meta.write("Program: %s\nFunction count: %d\n\n" % (program_name, len(functions)))

for fn in functions:
    if monitor.isCancelled():
        break
    name = fn.getName()
    entry = fn.getEntryPoint()
    body = fn.getBody()
    meta.write("%s\t%s\t%s\n" % (entry, name, fn.getSignature()))
    result = decomp.decompileFunction(fn, 60, monitor)
    code.write("\n/* ============================================================\n")
    code.write(" * %s @ %s\n" % (name, entry))
    code.write(" * Signature: %s\n" % fn.getSignature())
    code.write(" * ============================================================ */\n")
    if result and result.decompileCompleted():
        code.write(result.getDecompiledFunction().getC())
    else:
        code.write("/* DECOMPILATION FAILED: %s */\n" % (result.getErrorMessage() if result else "no result"))
    code.write("\n")

meta.close()
code.close()
decomp.dispose()
print("PicSay export complete: " + out_dir)
