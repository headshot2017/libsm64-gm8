# converts constants from libsm64 includes to gm8 extension maker constants format

def SOUND_ARG_LOAD(bank, playFlags, soundID, priority, flags2):
    return ((bank << 28) | (playFlags << 24) | (soundID << 16) | (priority << 8) | (flags2 << 4) | 1)

a = open("moreconst.txt").read().split("\n")
result = []
for b in a:
    if b.find(" //") >= 0: b = b[:b.find(" //")]
    b = "=".join(b.split()).replace(",=", ",")

    name = "const " + b.split("=")[0]
    val = b.split("=")[1]
    if val.find("SOUND_ARG_LOAD") >= 0:
        val = val.replace("(", ",").replace(")", "")
        args = val.split(",")[1:]
        for arg in args:
            args[args.index(arg)] = int(arg, 0)
        val = SOUND_ARG_LOAD(args[0], args[1], args[2], args[3], args[4])
    elif val.find("<<") >= 0:
        val = val.replace("(", "").replace(")", "").replace(" ", "")
        num = int(val.split("<<")[0])
        shift = int(val.split("<<")[1])
        val = num << shift
    else:
        val = int(val, 0)

    print "'%s' '%s'" % (name, val)
    result.append("%s=%s" % (name, val))

open("conv.txt", "w").write("\n".join(result))
