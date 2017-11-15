cmake_minimum_required(VERSION 3.7)
project(PlexilExec)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

set(SOURCE_FILES
        Assignment.cc
        Assignment.hh
        AssignmentNode.cc
        AssignmentNode.hh
        CommandNode.cc
        CommandNode.hh
        ExecConnector.cc
        ExecConnector.hh
        ExecListenerBase.hh
        InterfaceSchema.cc
        InterfaceSchema.hh
        LibraryCallNode.cc
        LibraryCallNode.hh
        ListNode.cc
        ListNode.hh
        Node.cc
        Node.hh
        NodeFactory.cc
        NodeFactory.hh
        NodeFunction.cc
        NodeFunction.hh
        NodeOperator.cc
        NodeOperator.hh
        NodeOperatorImpl.cc
        NodeOperatorImpl.hh
        NodeTimepointValue.cc
        NodeTimepointValue.hh
        NodeTransition.hh
        NodeVariableMap.cc
        NodeVariableMap.hh
        NodeVariables.cc
        NodeVariables.hh
        plan-utils.cc
        plan-utils.hh
        PlexilExec.cc
        PlexilExec.hh
        PlexilNodeType.cc
        PlexilNodeType.hh
        UpdateNode.cc
        UpdateNode.hh
        VariableConflictSet.cc
        VariableConflictSet.hh)

include_directories(
        ./
        ../
        ../utils
        ../value
        ../intfc
        ../expr
)

add_library(PlexilExec SHARED ${SOURCE_FILES})

target_link_libraries(PlexilExec wrap nsl m rt pthread dl)


