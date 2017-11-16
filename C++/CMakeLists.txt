cmake_minimum_required(VERSION 3.7)
project(ICAROUS)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-g -fPIC -m32")

set(ICAROUS_ROOT ${CMAKE_SOURCE_DIR})

set(LIBRARY_OUTPUT_PATH ${ICAROUS_ROOT}/lib)

set(ACCORD_LIB_SOURCE_FILES
        src/ACCoRD/ACCoRDConfig.cpp
        src/ACCoRD/AircraftState.cpp
        src/ACCoRD/AlertingMofN.cpp
        src/ACCoRD/AlertLevels.cpp
        src/ACCoRD/AlertThresholds.cpp
        src/ACCoRD/AziEquiProjection.cpp
        src/ACCoRD/Bands.cpp
        src/ACCoRD/BandsCore.cpp
        src/ACCoRD/BandsRange.cpp
        src/ACCoRD/BandsRegion.cpp
        src/ACCoRD/BoundingRectangle.cpp
        src/ACCoRD/CD2D.cpp
        src/ACCoRD/CD3D.cpp
        src/ACCoRD/CD3DTable.cpp
        src/ACCoRD/CDCylinder.cpp
        src/ACCoRD/CDII.cpp
        src/ACCoRD/CDIICore.cpp
        src/ACCoRD/CDIIPolygon.cpp
        src/ACCoRD/CDPolycarp.cpp
        src/ACCoRD/CDPolyIter.cpp
        src/ACCoRD/CDPolyIter2D.cpp
        src/ACCoRD/CDSI.cpp
        src/ACCoRD/CDSICore.cpp
        src/ACCoRD/CDSIPolygon.cpp
        src/ACCoRD/CDSS.cpp
        src/ACCoRD/CDSSCore.cpp
        src/ACCoRD/ColoredValue.cpp
        src/ACCoRD/ConflictBandsTwoTimes.cpp
        src/ACCoRD/ConflictData.cpp
        src/ACCoRD/Constants.cpp
        src/ACCoRD/CoordinationFilter.cpp
        src/ACCoRD/CR3D.cpp
        src/ACCoRD/Criteria.cpp
        src/ACCoRD/CriteriaCore.cpp
        src/ACCoRD/CriticalVectors.cpp
        src/ACCoRD/CriticalVectors2D.cpp
        src/ACCoRD/CriticalVectorsSI.cpp
        src/ACCoRD/CRSS.cpp
        src/ACCoRD/CRSSConfig.cpp
        src/ACCoRD/Daidalus.cpp
        src/ACCoRD/DaidalusFileWalker.cpp
        src/ACCoRD/DataServer.cpp
        src/ACCoRD/DCPAUrgencyStrategy.cpp
        src/ACCoRD/Debug.cpp
        src/ACCoRD/DebugSupport.cpp
        src/ACCoRD/DensityGrid.cpp
        src/ACCoRD/DensityGridAStarSearch.cpp
        src/ACCoRD/DensityGridMovingPolys.cpp
        src/ACCoRD/DensityGridMovingPolysEst.cpp
        src/ACCoRD/DensityGridTimed.cpp
        src/ACCoRD/Detection3DParameterReader.cpp
        src/ACCoRD/Detection3DParameterWriter.cpp
        src/ACCoRD/DetectionPolygonParameterReader.cpp
        src/ACCoRD/DetectionPolygonParameterWriter.cpp
        src/ACCoRD/ENUProjection.cpp
        src/ACCoRD/ErrorLog.cpp
        src/ACCoRD/FilePushTransmitter.cpp
        src/ACCoRD/FixedAircraftUrgencyStrategy.cpp
        src/ACCoRD/format.cpp
        src/ACCoRD/General3DState.cpp
        src/ACCoRD/GeneralPlan.cpp
        src/ACCoRD/GeneralPlanWriter.cpp
        src/ACCoRD/GeneralState.cpp
        src/ACCoRD/GeneralStateReader.cpp
        src/ACCoRD/GeneralStateWriter.cpp
        src/ACCoRD/GreatCircle.cpp
        src/ACCoRD/GsPlan.cpp
        src/ACCoRD/Horizontal.cpp
        src/ACCoRD/Integerval.cpp
        src/ACCoRD/IntentBands.cpp
        src/ACCoRD/IntentBandsCore.cpp
        src/ACCoRD/Interval.cpp
        src/ACCoRD/IntervalSet.cpp
        src/ACCoRD/KinematicAltBands.cpp
        src/ACCoRD/KinematicBands.cpp
        src/ACCoRD/KinematicBandsCore.cpp
        src/ACCoRD/KinematicBandsParameters.cpp
        src/ACCoRD/KinematicGsBands.cpp
        src/ACCoRD/KinematicIntegerBands.cpp
        src/ACCoRD/KinematicMultiBands.cpp
        src/ACCoRD/KinematicRealBands.cpp
        src/ACCoRD/Kinematics.cpp
        src/ACCoRD/KinematicsDist.cpp
        src/ACCoRD/KinematicsLatLon.cpp
        src/ACCoRD/KinematicsPosition.cpp
        src/ACCoRD/KinematicTrkBands.cpp
        src/ACCoRD/KinematicVsBands.cpp
        src/ACCoRD/LatLonAlt.cpp
        src/ACCoRD/LossData.cpp
        src/ACCoRD/MovingPolygon2D.cpp
        src/ACCoRD/MovingPolygon3D.cpp
        src/ACCoRD/NavPoint.cpp
        src/ACCoRD/NoneUrgencyStrategy.cpp
        src/ACCoRD/OrthographicProjection.cpp
        src/ACCoRD/ParameterData.cpp
        src/ACCoRD/ParameterEntry.cpp
        src/ACCoRD/Plan.cpp
        src/ACCoRD/PlanIO.cpp
        src/ACCoRD/PlanReader.cpp
        src/ACCoRD/PlanUtil.cpp
        src/ACCoRD/PlanWriter.cpp
        src/ACCoRD/Point.cpp
        src/ACCoRD/Poly2D.cpp
        src/ACCoRD/Poly3D.cpp
        src/ACCoRD/Polycarp3D.cpp
        src/ACCoRD/PolycarpAcceptablePolygon.cpp
        src/ACCoRD/PolycarpContain.cpp
        src/ACCoRD/PolycarpDetection.cpp
        src/ACCoRD/PolycarpDoubleQuadratic.cpp
        src/ACCoRD/PolycarpEdgeProximity.cpp
        src/ACCoRD/PolycarpQuadMinmax.cpp
        src/ACCoRD/PolycarpResolution.cpp
        src/ACCoRD/PolyPath.cpp
        src/ACCoRD/PolyUtil.cpp
        src/ACCoRD/Position.cpp
        src/ACCoRD/PositionUtil.cpp
        src/ACCoRD/Priority.cpp
        src/ACCoRD/PriorityCore.cpp
        src/ACCoRD/ProjectedKinematics.cpp
        src/ACCoRD/Projection.cpp
        src/ACCoRD/Route.cpp
        src/ACCoRD/SafetyBuffers.cpp
        src/ACCoRD/SeparatedInput.cpp
        src/ACCoRD/SeparatedOutput.cpp
        src/ACCoRD/SequenceReader.cpp
        src/ACCoRD/SimpleMovingPoly.cpp
        src/ACCoRD/SimpleNoPolarProjection.cpp
        src/ACCoRD/SimplePoly.cpp
        src/ACCoRD/SimpleProjection.cpp
        src/ACCoRD/StateReader.cpp
        src/ACCoRD/StateVector.cpp
        src/ACCoRD/StateWriter.cpp
        src/ACCoRD/string_util.cpp
        src/ACCoRD/TangentLine.cpp
        src/ACCoRD/TargetUrgency.cpp
        src/ACCoRD/TCAS2D.cpp
        src/ACCoRD/TCAS3D.cpp
        src/ACCoRD/TCASTable.cpp
        src/ACCoRD/TcpData.cpp
        src/ACCoRD/TrafficCoreState.cpp
        src/ACCoRD/TrafficState.cpp
        src/ACCoRD/TrajGen.cpp
        src/ACCoRD/TrajTemplates.cpp
        src/ACCoRD/TripleBands.cpp
        src/ACCoRD/TurnGeneration.cpp
        src/ACCoRD/Units.cpp
        src/ACCoRD/Util.cpp
        src/ACCoRD/Vect2.cpp
        src/ACCoRD/Vect3.cpp
        src/ACCoRD/Vect4.cpp
        src/ACCoRD/VectFuns.cpp
        src/ACCoRD/Velocity.cpp
        src/ACCoRD/Vertical.cpp
        src/ACCoRD/WCV_HZ.cpp
        src/ACCoRD/WCV_TAUMOD.cpp
        src/ACCoRD/WCV_TCOA.cpp
        src/ACCoRD/WCV_TCPA.cpp
        src/ACCoRD/WCV_TEP.cpp
        src/ACCoRD/WCV_tvar.cpp
        src/ACCoRD/WCV_VMOD.cpp
        src/ACCoRD/WCVTable.cpp
        src/ACCoRD/WeatherUtil.cpp)

set(ICAROUS_LIB_SOURCE_FILES
        src/ICAROUS/AircraftData.cpp
        src/ICAROUS/ConflictDetection.cpp
        src/ICAROUS/FlightManagementSystem.cpp
        src/ICAROUS/GenericObject.cpp
        src/ICAROUS/GeoFence.cpp
        src/ICAROUS/Icarous.cpp
        src/ICAROUS/Port.cpp
        src/ICAROUS/QuadFMS.cpp
        src/ICAROUS/Resolution.cpp
        src/ICAROUS/RRT.cpp)

set(MAVLINK_INTERFACE_SOURCE
        src/INTERFACE/MavlinkInterface.cpp)

include_directories(include/ACCoRD include/ICAROUS include/PLEXIL libs/mavlink include/INTERFACE plexil-4/include)

add_library(ACCoRD SHARED ${ACCORD_LIB_SOURCE_FILES})

add_library(ICAROUS SHARED ${ICAROUS_LIB_SOURCE_FILES})

add_library(MavlinkInterface SHARED ${MAVLINK_INTERFACE_SOURCE})

target_link_libraries(ICAROUS ACCoRD)

add_executable(icarous src/LaunchIcarous.cpp)
target_link_libraries(icarous ICAROUS ACCoRD MavlinkInterface pthread)