import sys
import os

numApps = len(sys.argv) - 3
approot = sys.argv[1].rstrip('/')
outputloc = sys.argv[2].rstrip('/')

if not os.path.exists(outputloc):
    os.makedirs(outputloc)

outfp = open(outputloc + '/cfe_es_startup.scr', 'w')
appcfg = {}
for app in sys.argv[3:]:
    appcfg[app] = {}
    infp = open(approot+'/'+app+'/app.cfg', 'r')
    while True:
        line = infp.readline().strip('\n')
        if line != "":
            line = line.split('=')
            field = line[0]
            value = line[1]
            appcfg[app][field] = value
        else:
            break

    infp.close()

for app in sys.argv[3:]:
    appdata = appcfg[app]
    try:
        if(int(appdata['SHOULD_START']) == 1):
            print("-- Writing config for app: %s"%(app))
            outfp.write(appdata['OBJ_TYPE']+', ')
            outfp.write(appdata['APP_PATH']+', ')
            outfp.write(appdata['ENTRY_PT']+', ')
            outfp.write(appdata['CFE_NAME']+', ')
            outfp.write(appdata['PRIORITY']+', ')
            outfp.write(appdata['STACK_SIZE']+', ')
            outfp.write(appdata['LOAD_ADDR']+', ')
            outfp.write(appdata['EXCEPT_ACT']+';\n')
    except:
        print("Error in config: Check configuration for %s"%(app))
   






