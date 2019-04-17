from argparse import ArgumentParser
from datetime import datetime as dt
import pystache 
import os

parser = ArgumentParser(description=__doc__)

parser.add_argument('--APP_NAME',help='Name of the cFS application',default='SampleApp')
parser.add_argument('--AUTHOR',help='Author name',default=' ')
args = parser.parse_args()

os.mkdir(args.APP_NAME)
os.mkdir(args.APP_NAME+'/fsw')
os.mkdir(args.APP_NAME+'/fsw/src')
os.mkdir(args.APP_NAME+'/fsw/tables')

today = dt.now()
date = str(today.month)+'/'+str(today.day)+'/'+str(today.year)

templ_args = {'APP_NAME':args.APP_NAME,
              'APP_NAMEU':args.APP_NAME.upper(),
              'AUTHOR': args.AUTHOR,
              'DATE': date}

# Read the CMakeLists.txt
fp1 = open('sample_app/CMakeLists.txt','r')
template1 = fp1.read()
fp2 = open(args.APP_NAME+'/CMakeLists.txt','w')
fp2.write(pystache.render(template1,templ_args))
fp1.close()
fp2.close()

# Read the .h file
fp1 = open('sample_app/sample_app.h','r')
template1 = fp1.read()
fp2 = open(args.APP_NAME+'/fsw/src/'+args.APP_NAME+'.h','w')
fp2.write(pystache.render(template1,templ_args))
fp1.close()
fp2.close()

# Read the .c file
fp1 = open('sample_app/sample_app.c','r')
template1 = fp1.read()
fp2 = open(args.APP_NAME+'/fsw/src/'+args.APP_NAME+'.c','w')
fp2.write(pystache.render(template1,templ_args))
fp1.close()
fp2.close()

# Read the tbl file
fp1 = open('sample_app/sample_app_tbl.h','r')
template1 = fp1.read()
fp2 = open(args.APP_NAME+'/fsw/src/'+args.APP_NAME+'_tbl.h','w')
fp2.write(pystache.render(template1,templ_args))
fp1.close()
fp2.close()

# Read the tbl .c file
fp1 = open('sample_app/sample_app_tbl.c','r')
template1 = fp1.read()
fp2 = open(args.APP_NAME+'/fsw/tables/'+args.APP_NAME+'_tbl.c','w')
fp2.write(pystache.render(template1,templ_args))
fp1.close()
fp2.close()


