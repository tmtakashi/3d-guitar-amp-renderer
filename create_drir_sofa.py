from netCDF4 import Dataset
import time
import numpy as np
import soundfile as sf
import os
from pathlib import Path

#----------Create it----------#

filePath = "data/Marshall1960ADRIR.sofa"
# Need to delete it first if file already exists
if os.path.exists(filePath):
    os.remove(filePath)
rootgrp = Dataset(filePath, 'w', format='NETCDF4')


#----------Required Attributes----------#

rootgrp.Conventions = 'SOFA'
rootgrp.Version = '1.0'
rootgrp.SOFAConventions = 'GeneralFIR'
rootgrp.SOFAConventionsVersion = '1.0'
rootgrp.APIName = 'pysofaconventions'
rootgrp.APIVersion = '0.1'
rootgrp.AuthorContact = 'tmtakashi7@gmail.com'
rootgrp.Organization = 'Kyushu University'
rootgrp.License = 'WTFPL - Do What the Fuck You Want to Public License'
rootgrp.DataType = 'FIR'
rootgrp.RoomType = 'reverberant'
rootgrp.DateCreated = time.ctime(time.time())
rootgrp.DateModified = time.ctime(time.time())
rootgrp.Title = 'Marshall1960ADRIR'


#----------Required Dimensions----------#

m = 1
n = 262144
r = 80
e = 1
i = 1
c = 3
rootgrp.createDimension('M', m)
rootgrp.createDimension('N', n)
rootgrp.createDimension('E', e)
rootgrp.createDimension('R', r)
rootgrp.createDimension('I', i)
rootgrp.createDimension('C', c)


#----------Required Variables----------#
sourcePositionVar = rootgrp.createVariable(
    'SourcePosition',        'f8',   ('I', 'C'))
sourcePositionVar.Units = 'metre'
sourcePositionVar.Type = 'cartesian'
sourcePositionVar[:] = np.zeros(c)

sourceUpVar = rootgrp.createVariable(
    'SourceUp',              'f8',   ('I', 'C'))
sourceUpVar.Units = 'metre'
sourceUpVar.Type = 'cartesian'
sourceUpVar[:] = np.asarray([0, 0, 1])

sourceViewVar = rootgrp.createVariable(
    'SourceView',            'f8',   ('I', 'C'))
sourceViewVar.Units = 'metre'
sourceViewVar.Type = 'cartesian'
sourceViewVar[:] = np.asarray([1, 0, 0])

# receiver position
positions = np.load('data/fullerene_polar.npy')
positions[:, :-1] = np.rad2deg(positions[:, :-1])
receiverPositionVar = rootgrp.createVariable(
    'ReceiverPosition',  'f8',   ('R', 'C', 'I'))
receiverPositionVar.Units = 'degree, degree, metre'
receiverPositionVar.Type = 'spherical'
receiverPositionVar[:] = positions[:, :, np.newaxis]

samplingRateVar = rootgrp.createVariable('Data.SamplingRate', 'f8',   ('I'))
samplingRateVar.Units = 'hertz'
samplingRateVar[:] = 48000

# load irs
irs = np.zeros((r, n))
for i, f in enumerate(sorted(Path('./data').glob("*.wav"))):
    ir, _ = sf.read(f)
    irs[i, :] = ir
dataIRVar = rootgrp.createVariable('Data.IR', 'f8', ('M', 'R', 'N'))
dataIRVar[:] = irs[np.newaxis, :, :]

#----------Close it----------#

rootgrp.close()
