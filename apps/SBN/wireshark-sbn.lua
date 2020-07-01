-- LUA code for a Wireshark protocol dissector for SBN messages.
-- TODO: would be nice to auto-decode CCSDS payloads in "app" messages.
--
-- For install instructions, see:
--         https://www.wireshark.org/docs/wsdg_html_chunked/wsluarm.html

-- change below or use "Decode As..." to decode SBN messages.

local sbn_port_udp = 2234
local sbn_port_tcp = 2234

local proto_sbn = Proto("cfs_sbn", "Core Flight Software - Software Bus Networking")

local proto_sbn_types = {
    [0] = "NOMSG",
    [1] = "SUB",
    [2] = "UNSUB",
    [3] = "APP",
    [4] = "PROTO"
}

local proto_sbn_msgsz = ProtoField.uint16("cfs_sbn.MsgSz", "MsgSz", base.DEC)

local proto_sbn_type =
    ProtoField.uint8("cfs_sbn.MsgType", "MsgType", base.DEC, proto_sbn_types)

local proto_sbn_cpuid = ProtoField.uint32("cfs_sbn.CPUID", "CPUID", base.DEC)

local proto_sbn_version =
    ProtoField.string("cfs_sbn.sub.Version", "Version", base.ASCII)

local proto_sbn_sub_cnt =
    ProtoField.uint16("cfs_sbn.sub.Cnt", "Sub Count", base.DEC)

local proto_sbn_sub_mid =
    ProtoField.uint16("cfs_sbn.sub.MID", "Subscription", base.HEX)

proto_sbn.fields = {
    proto_sbn_msgsz,
    proto_sbn_type,
    proto_sbn_cpuid,
    proto_sbn_sub,
    proto_sbn_version,
    proto_sbn_sub_cnt,
    proto_sbn_sub_mid
}

function proto_sbn.dissector(buffer, pinfo, tree)
    if buffer:len() == 0 then return end
    pinfo.cols.protocol = proto_sbn.name

    local subtree = tree:add(proto_sbn, buffer(), "SBN Data")
    local offset = 0

    subtree:add(proto_sbn_msgsz, buffer(offset, 2))
    offset = offset + 2

    subtree:add(proto_sbn_type, buffer(offset, 1))
    local msgtype = buffer(offset, 1):uint()
    offset = offset + 1

    subtree:add(proto_sbn_cpuid, buffer(3, 4))
    offset = offset + 4

    if msgtype == 1 then -- sub
        subtree:add(proto_sbn_version, buffer(offset, 48))
        offset = offset + 48
    end

    if msgtype == 1 or msgtype == 2 then -- sub/unsub
        subtree:add(proto_sbn_sub_cnt, buffer(offset, 2))
        local sub_cnt = buffer(offset, 2):uint()
        offset = offset + 2

        for i = 0, sub_cnt - 1, 1 do
            subtree:add(proto_sbn_sub_mid, buffer(offset, 2))
            offset = offset + 2
        end
    end

    if msgtype == 3 then
        local ccsds = Dissector.get("data")
        ccsds:call(buffer(offset):tvb(), pinfo, subtree)
    end
end

DissectorTable.get("udp.port"):add(sbn_port_udp, proto_sbn)
DissectorTable.get("tcp.port"):add(sbn_port_tcp, proto_sbn)
