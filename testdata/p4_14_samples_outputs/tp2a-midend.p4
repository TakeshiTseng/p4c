#include <core.p4>
#include <v1model.p4>

header data_t {
    bit<32> f1;
    bit<32> f2;
    bit<32> f3;
    bit<32> f4;
    bit<32> b1;
    bit<32> b2;
    bit<32> b3;
    bit<32> b4;
}

struct metadata {
}

struct headers {
    @name("data") 
    data_t data;
}

parser ParserImpl(packet_in packet, out headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    @name("start") state start {
        packet.extract<data_t>(hdr.data);
        transition accept;
    }
}

control ingress(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    @name("NoAction_1") action NoAction() {
    }
    @name("NoAction_2") action NoAction_0() {
    }
    @name("NoAction_3") action NoAction_5() {
    }
    @name("NoAction_4") action NoAction_6() {
    }
    @name("setb1") action setb1_0(bit<32> val) {
        hdr.data.b1 = val;
    }
    @name("noop") action noop_0() {
    }
    @name("noop") action noop_4() {
    }
    @name("noop") action noop_5() {
    }
    @name("noop") action noop_6() {
    }
    @name("setb3") action setb3_0(bit<32> val) {
        hdr.data.b3 = val;
    }
    @name("setb2") action setb2_0(bit<32> val) {
        hdr.data.b2 = val;
    }
    @name("setb4") action setb4_0(bit<32> val) {
        hdr.data.b4 = val;
    }
    @name("A1") table A1() {
        actions = {
            setb1_0();
            noop_0();
            NoAction();
        }
        key = {
            hdr.data.f1: ternary;
        }
        default_action = NoAction();
    }
    @name("A2") table A2() {
        actions = {
            setb3_0();
            noop_4();
            NoAction_0();
        }
        key = {
            hdr.data.b1: ternary;
        }
        default_action = NoAction_0();
    }
    @name("B1") table B1() {
        actions = {
            setb2_0();
            noop_5();
            NoAction_5();
        }
        key = {
            hdr.data.f2: ternary;
        }
        default_action = NoAction_5();
    }
    @name("B2") table B2() {
        actions = {
            setb4_0();
            noop_6();
            NoAction_6();
        }
        key = {
            hdr.data.b2: ternary;
        }
        default_action = NoAction_6();
    }
    apply {
        if (hdr.data.b1 == 32w0) {
            A1.apply();
            A2.apply();
        }
        B1.apply();
        B2.apply();
    }
}

control egress(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    apply {
    }
}

control DeparserImpl(packet_out packet, in headers hdr) {
    apply {
        packet.emit<data_t>(hdr.data);
    }
}

control verifyChecksum(in headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    apply {
    }
}

control computeChecksum(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    apply {
    }
}

V1Switch<headers, metadata>(ParserImpl(), verifyChecksum(), ingress(), egress(), computeChecksum(), DeparserImpl()) main;