

def ParseDaidalusConfiguration(filename):
    from ctypes import Structure, CDLL, c_char_p, c_int, c_char, c_double
    lib = CDLL('libUtils.so')
    class ParsedParam(Structure):
        _fields_=[
            ("key",c_char*50),
            ("valueString",c_char*50),
            ("unitString",c_char*10),
            ("value",c_double)
        ]
    
    lib.ParseParameterFile.argtype = [c_char_p,ParsedParam*100] 
    lib.ParseParameterFile.restype = c_int

    ParamsArray = ParsedParam * 100
    params = ParamsArray()
    n = lib.ParseParameterFile(c_char_p(filename.encode('utf-8')),params)
    DaidalusParam = {}
    for param in params:
        key = param.key.decode('utf-8')
        if key != "":
            DaidalusParam[key] =[param.value, param.valueString.decode('utf-8')]

    return DaidalusParam


def ParseAccordFormat(filename):
    fp = open(filename,'r')
    stream = fp.read().split('\n')
    data = {}
    
    headers = []
    header = None
    headerIndex = 0
    headerLineNo = 0
    ignoreUnits = True
    
    for k,line in enumerate(stream):
       val = line.strip(' ')
       if len(val) == 0:
          continue
    
       if val[0] == "#":
          # ignore comments:
          if k - headerLineNo > 1:
              header = None
          continue
       else:
          entry = val.split('#')[0].split('=')
          if len(entry) == 2:
              keyval = entry
              # simple key value pair
              key = keyval[0].strip(' ').lower()
              value = keyval[1].strip(' ')
              data[key] = value 
          else:
              # sequence of values
              fields = entry[0].split(',')
              for i in range(len(fields)):
                  fields[i] = fields[i].strip(' ')
    
              valueEntry = True
              if header is None:
                  header = fields[0]
                  headers.append(fields)
                  valueEntry = False
                  headerIndex = len(headers)-1
                  headerLineNo = k
                  for field in fields:
                      data[field] = []
              else:
                  for i,hd in enumerate(headers):
                     if fields[0] == hd[0]:
                         header=hd[0]
                         valueEntry = False
                         headerIndex = i
                         headerLineNo = k
                         break
    
              if valueEntry:
                  for i,val in enumerate(fields):
                      data[headers[headerIndex][i]].append(val)
                  
    return data 

def GenMavlinkFp(plan,filename):
    fp = open(filename,'x+')
    header = "QGC WPL 1.0"
    speed = 0
    index = 0
    autocontinue = 1
    data = header+"\n"
    for i,wp in enumerate(plan):
        params = [0.0 for i in range(7)]
        currentWP = 0 
        coordFrame = 3
        if i > 0 and abs(plan[i][3] - speed) > 1e-3:
            command = 178
            speed = plan[i][3]
            params[1] = speed
            entry = [index,currentWP,coordFrame,command,*params,autocontinue]
            index += 1
            data += ' '.join(map(str,entry))+"\n"

        command = 16
        params[1] = 0.0
        params[4] = wp[0]
        params[5] = wp[1]
        params[6] = wp[2]
        entry = [index,currentWP,coordFrame,command,*params,autocontinue]
        index += 1
        data += ' '.join(map(str,entry))+"\n"
    fp.write(data)
    fp.close()

def CreateIcarousParamFile(daaparams,filename):
    import math
    # Default icarous parameter values
    mps2knots = 1.94384
    mps2fpm  = 196.85
    m2ft = 3.28084
    icParams = {}
    icParams["TRAFFIC_SRC"]=0.0
    icParams["RES_TYPE"]=2.0
    icParams["LOGDAADATA"]=1.000000
    icParams["LOOKAHEAD_TIME"]=daaparams['lookahead_time'][0]
    icParams["LEFT_TRK"]=math.degrees(daaparams['left_hdir'][0])
    icParams["RIGHT_TRK"]=math.degrees(daaparams['right_hdir'][0])
    icParams["MIN_GS"]=daaparams['min_hs'][0]*mps2knots
    icParams["MAX_GS"]=daaparams['max_hs'][0]*mps2knots
    icParams["MIN_VS"]=daaparams['min_vs'][0]*mps2fpm
    icParams["MAX_VS"]=daaparams['max_vs'][0]*mps2fpm
    icParams["MIN_ALT"]=daaparams['min_alt'][0]*m2ft
    icParams["MAX_ALT"]=daaparams['max_alt'][0]*m2ft
    icParams["TRK_STEP"]=math.degrees(daaparams['step_hdir'][0])
    icParams["GS_STEP"]=daaparams['step_hs'][0]*mps2knots
    icParams["VS_STEP"]=daaparams['step_vs'][0]*mps2fpm
    icParams["ALT_STEP"]=daaparams['step_alt'][0]*m2ft
    icParams["HORIZONTAL_ACCL"]=daaparams['horizontal_accel'][0]
    icParams["VERTICAL_ACCL"]=daaparams['vertical_accel'][0]
    icParams["TURN_RATE"]=math.degrees(daaparams['turn_rate'][0])
    icParams["BANK_ANGLE"]=math.degrees(daaparams['bank_angle'][0])
    icParams["VERTICAL_RATE"]=daaparams['vertical_rate'][0]*mps2fpm
    icParams["RECOV_STAB_TIME"]=daaparams['recovery_stability_time'][0]
    icParams["PERSIST_TIME"]=3.000000
    icParams["MIN_HORIZ_RECOV"]=daaparams['min_horizontal_recovery'][0]*m2ft
    icParams["MIN_VERT_RECOV"]=daaparams['min_vertical_recovery'][0]*m2ft
    icParams["RECOVERY_TRK"]=(1 if daaparams['recovery_hdir'][1] == 'true' else 0)
    icParams["RECOVERY_GS"]=(1 if daaparams['recovery_hs'][1] == 'true' else 0)
    icParams["RECOVERY_VS"]=(1 if daaparams['recovery_vs'][1] == 'true' else 0)
    icParams["RECOVERY_ALT"]=(1 if daaparams['recovery_alt'][1] == 'true' else 0)
    icParams["CA_BANDS"]=(1 if daaparams['ca_bands'][1] == 'true' else 0)
    icParams["CA_FACTOR"]=daaparams['ca_factor'][0]
    icParams["HORIZONTAL_NMAC"]=daaparams['horizontal_nmac'][0]*m2ft
    icParams["VERTICAL_NMAC"]=daaparams['vertical_nmac'][0]*m2ft
    icParams["CONFLICT_CRIT"]=(1 if daaparams['conflict_crit'][1]  == 'true' else 0)
    icParams["RECOVERY_CRIT"]=(1 if daaparams['recovery_crit'][1] == 'true' else 0)
    icParams["CONTOUR_THR"]=math.degrees(daaparams['contour_thr'][0])
    icParams["AL_1_ALERT_T"]=daaparams['default_alert_1_alerting_time'][0]
    icParams["AL_1_E_ALERT_T"]=daaparams['default_alert_1_early_alerting_time'][0]
    icParams["AL_1_SPREAD_ALT"]=daaparams['default_alert_1_spread_alt'][0]
    icParams["AL_1_SPREAD_GS"]=daaparams['default_alert_1_spread_hs'][0]
    icParams["AL_1_SPREAD_TRK"]=daaparams['default_alert_1_spread_hdir'][0]
    icParams["AL_1_SPREAD_VS"]=daaparams['default_alert_1_spread_vs'][0]
    icParams["DET_1_WCV_DTHR"]=daaparams['default_det_1_wcv_dthr'][0]*m2ft
    icParams["DET_1_WCV_TCOA"]=daaparams['default_det_1_wcv_tcoa'][0]
    icParams["DET_1_WCV_TTHR"]=daaparams['default_det_1_wcv_tthr'][0]
    icParams["DET_1_WCV_ZTHR"]=daaparams['default_det_1_wcv_zthr'][0]*m2ft
    icParams["OBSBUFFER"]=1.000000
    icParams["MAXCEILING"]=50.000000
    icParams["CLMB_SPEED"]=1.000000
    icParams["ALT_DH"]=1.000000
    icParams["XTRKDEV"]=15.000000
    icParams["RETURN_WP"]=1.000000
    icParams["LOOKAHEAD"]=0.000000
    icParams["HTHRESHOLD"]=10.000000
    icParams["VTHRESHOLD"]=5.000000
    icParams["HSTEPBACK"]=20.000000
    icParams["VSTEPBACK"]=20.000000
    icParams["MAX_MERGE_SPEED"]=8.0
    icParams["MIN_MERGE_SPEED"]=1.0
    icParams["CORRIDOR_WIDTH"]=0.0
    icParams["ENTRY_RADIUS"]=60.0
    icParams["COORD_ZONE"]=90.0
    icParams["SCHEDULE_ZONE"]=70.0
    icParams["MIN_SEP_DIST"]=30.0
    icParams["MIN_SEP_TIME"]=20.0
    icParams["MAX_TURN_RADIUS"]=0.0
    icParams["START_MERGE_PT"]=1
    icParams["DEF_WP_SPEED"]=1.0
    icParams["CAP_R_SCALING"]=2.0
    icParams["GUID_R_SCALING"]=2.0
    icParams["CLIMB_ANGLE"]=75
    icParams["CLIMB_ANGLE_VR"]=10
    icParams["CLIMB_ANGLE_HR"]=10
    icParams["CLIMB_RATE_GAIN"]=0.2
    icParams["MAX_CAP"]=15
    icParams["MIN_CAP"]=1
    icParams["YAW_FORWARD"]=1
    icParams["TURNRATE_GAIN"]=5.0
    
    data = ""
    for val in icParams:
        data += val + " " + str(icParams[val]) + "\n"

    fp = open(filename,'x+')
    fp.write(data)

def GenDaidalusFile(daaparams,filename):
    data = ""
    for val in daaparams:
        data += val + " = " + daaparams[val] + "\n"
    fp = open(filename,'x+')
    fp.write(data)

def GetScenariosFromAccordInput(filename,datalocation):
    # Get current timestamp for folder name
    from datetime import datetime
    import os,shutil
    now = datetime.now()
    cwd = os.getcwd()
    date_time = now.strftime("%m_%d_%Y_%H_%M_%S")
    # Parse the input file
    accordInput = ParseAccordFormat(filename)
    
    numAC = len(accordInput['FP'])
    defaultDaaFile = accordInput['default_daa_conf']
    accordInput['wps'] = list(map(int,accordInput['wps']))
    
    # Construct the scenario configuration
    icInputConfig = {}
    icInputConfig['scenarios'] = []
    scenario = {}
    scenario['name'] = "Encounter"
    scenario['time_limit'] = 300
    scenario['vehicles'] = []
    icInputConfig['scenarios'].append(scenario)
    
    datalocation = cwd + "/" + datalocation + '/.%s_%s'%(scenario['name'],date_time)   
    os.makedirs(datalocation,exist_ok=True)
    daaFile = ""
    for i in range(numAC):
        equipped = accordInput['equipped'][i]
        acid = accordInput['ac_id'][i]
        vehicle = {}    
        vehicle['name'] = acid
        vehicle['waypoint_file'] = datalocation + '/waypoint_%s.txt'%(vehicle['name'])
        vehicle['geofence_file'] = None
        vehicle['parameter_file'] = datalocation + '/parameters_%s.parm'%(vehicle['name'])
        vehicle['daa_file'] = datalocation + '/daidalusconfig_%s.txt'%(vehicle['name']) 
        vehicle['param_adjustments'] = {}
        vehicle['traffic'] = []
        vehicle['python'] = True
    
        if equipped == 'true':
            vehicle['transmitter'] = 'GroundTruth'
            vehicle['receiver'] = 'GroundTruth'
        else:
            vehicle['transmitter'] = 'GroundTruth'
            vehicle['receiver'] = None
    
    
        numWP = accordInput['wps'][i]
        starti = sum(accordInput['wps'][:i])
        FP = []
        for j in range(starti,starti+numWP):
            lat = float(accordInput['lat'][j])
            lon = float(accordInput['lon'][j])
            alt = float(accordInput['alt'][j])
            speed = float(accordInput['speed'][j])
            FP.append([lat,lon,alt,speed*0.5144])
    
        GenMavlinkFp(FP,vehicle['waypoint_file'])
    
        daaFile = accordInput['daa_conf'][i]
        if daaFile == "_":
            daaFile = defaultDaaFile
        daaParams = ParseDaidalusConfiguration(daaFile)
        CreateIcarousParamFile(daaParams,vehicle['parameter_file'])
        shutil.copyfile(daaFile,vehicle['daa_file'])
        icInputConfig['scenarios'][0]['vehicles'].append(vehicle)

    return icInputConfig
