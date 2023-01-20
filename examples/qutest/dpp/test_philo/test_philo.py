# test-script for QUTest unit testing harness
# see https://www.state-machine.com/qtools/qutest.html

# preamble...
def on_reset():
    expect_pause()
    continue_test()
    expect_run()
    glb_filter(GRP_ALL)
    loc_filter(IDS_ALL, -IDS_AO)
    ao_filter("Philo::inst[2]")
    current_obj(OBJ_SM_AO, "Philo::inst[2]")
    current_obj(OBJ_TE, "Philo::inst[2].m_timeEvt")

# tests...
test("Philo-thinking tick")
tick()
expect("           Tick<0>  Ctr=*")
expect("           TE0-ADis Obj=Philo::inst[2].m_timeEvt,AO=Philo::inst[2]")
expect("@timestamp TE0-Post Obj=Philo::inst[2].m_timeEvt,Sig=TIMEOUT_SIG,AO=Philo::inst[2]")
expect("@timestamp AO-Post  Sdr=QS_RX,Obj=Philo::inst[2],Evt<Sig=TIMEOUT_SIG,*")
expect("@timestamp AO-GetL  Obj=Philo::inst[2],Evt<Sig=TIMEOUT_SIG,*")
expect("@timestamp Disp===> Obj=Philo::inst[2],Sig=TIMEOUT_SIG,State=Philo::thinking")
expect("@timestamp TE0-DisA Obj=Philo::inst[2].m_timeEvt,AO=Philo::inst[2]")
expect("===RTC===> St-Exit  Obj=Philo::inst[2],State=Philo::thinking")
expect("@timestamp MP-Get   Obj=EvtPool1,*")
expect("@timestamp QF-New   Sig=HUNGRY_SIG,*")
expect("@timestamp QF-gc    Evt<Sig=HUNGRY_SIG,*")
expect("@timestamp MP-Put   Obj=EvtPool1,*")
expect("===RTC===> St-Entry Obj=Philo::inst[2],State=Philo::hungry")
expect("@timestamp ===>Tran Obj=Philo::inst[2],Sig=TIMEOUT_SIG,State=Philo::thinking->Philo::hungry")
expect("@timestamp Trg-Done QS_RX_TICK")

test("Philo-hungry publish EAT", NORESET)
publish("EAT_SIG", pack("<B", 2))
expect("@timestamp MP-Get   Obj=EvtPool1,*")
expect("@timestamp QF-New   Sig=EAT_SIG,*")
expect("@timestamp QF-Pub   Sdr=QS_RX,Evt<Sig=EAT_SIG,*")
expect("@timestamp AO-Post  Sdr=QS_RX,Obj=Philo::inst[2],Evt<Sig=EAT_SIG,*")
expect("@timestamp QUTEST_ON_POST EAT_SIG,Obj=Philo::inst[2] 2")
expect("@timestamp QF-gc?   Evt<Sig=EAT_SIG,*")
expect("@timestamp AO-GetL  Obj=Philo::inst[2],Evt<Sig=EAT_SIG,*")
expect("@timestamp Disp===> Obj=Philo::inst[2],Sig=EAT_SIG,State=Philo::hungry")
expect("@timestamp BSP_CALL BSP::random *")
expect("@timestamp TE0-Arm  Obj=Philo::inst[2].m_timeEvt,AO=Philo::inst[2],Tim=*,Int=0")
expect("===RTC===> St-Entry Obj=Philo::inst[2],State=Philo::eating")
expect("@timestamp ===>Tran Obj=Philo::inst[2],Sig=EAT_SIG,State=Philo::hungry->Philo::eating")
expect("@timestamp QF-gc    Evt<Sig=EAT_SIG,*")
expect("@timestamp MP-Put   Obj=EvtPool1,*")
expect("@timestamp Trg-Done QS_RX_EVENT")

test("Philo-eating tick", NORESET)
tick()
expect("           Tick<0>  Ctr=*")
expect("           TE0-ADis Obj=Philo::inst[2].m_timeEvt,AO=Philo::inst[2]")
expect("@timestamp TE0-Post Obj=Philo::inst[2].m_timeEvt,Sig=TIMEOUT_SIG,AO=Philo::inst[2]")
expect("@timestamp AO-Post  Sdr=QS_RX,Obj=Philo::inst[2],Evt<Sig=TIMEOUT_SIG,Pool=0,Ref=0>,*")
expect("@timestamp AO-GetL  Obj=Philo::inst[2],Evt<Sig=TIMEOUT_SIG,Pool=0,Ref=0>")
expect("@timestamp Disp===> Obj=Philo::inst[2],Sig=TIMEOUT_SIG,State=Philo::eating")
expect("@timestamp TE0-DisA Obj=Philo::inst[2].m_timeEvt,AO=Philo::inst[2]")
expect("@timestamp MP-Get   Obj=EvtPool1,*")
expect("@timestamp QF-New   Sig=DONE_SIG,*")
expect("@timestamp QF-Pub   Sdr=Philo::inst[2],Evt<Sig=DONE_SIG,Pool=1,Ref=0>")
expect("@timestamp QF-gc    Evt<Sig=DONE_SIG,Pool=1,Ref=1>")
expect("@timestamp MP-Put   Obj=EvtPool1,*")
expect("===RTC===> St-Exit  Obj=Philo::inst[2],State=Philo::eating")
expect("@timestamp BSP_CALL BSP::random *")
expect("@timestamp TE0-Arm  Obj=Philo::inst[2].m_timeEvt,AO=Philo::inst[2],Tim=*,Int=0")
expect("===RTC===> St-Entry Obj=Philo::inst[2],State=Philo::thinking")
expect("@timestamp ===>Tran Obj=Philo::inst[2],Sig=TIMEOUT_SIG,State=Philo::eating->Philo::thinking")
expect("@timestamp Trg-Done QS_RX_TICK")

test("Philo-thinking tick(2)", NORESET)
tick()
expect("           Tick<0>  Ctr=*")
expect("           TE0-ADis Obj=Philo::inst[2].m_timeEvt,AO=Philo::inst[2]")
expect("@timestamp TE0-Post Obj=Philo::inst[2].m_timeEvt,Sig=TIMEOUT_SIG,AO=Philo::inst[2]")
expect("@timestamp AO-Post  Sdr=QS_RX,Obj=Philo::inst[2],Evt<Sig=TIMEOUT_SIG,*")
expect("@timestamp AO-GetL  Obj=Philo::inst[2],Evt<Sig=TIMEOUT_SIG,*")
expect("@timestamp Disp===> Obj=Philo::inst[2],Sig=TIMEOUT_SIG,State=Philo::thinking")
expect("@timestamp TE0-DisA Obj=Philo::inst[2].m_timeEvt,AO=Philo::inst[2]")
expect("===RTC===> St-Exit  Obj=Philo::inst[2],State=Philo::thinking")
expect("@timestamp MP-Get   Obj=EvtPool1,*")
expect("@timestamp QF-New   Sig=HUNGRY_SIG,*")
expect("@timestamp QF-gc    Evt<Sig=HUNGRY_SIG,*")
expect("@timestamp MP-Put   Obj=*")
expect("===RTC===> St-Entry Obj=Philo::inst[2],State=Philo::hungry")
expect("@timestamp ===>Tran Obj=Philo::inst[2],Sig=TIMEOUT_SIG,State=Philo::thinking->Philo::hungry")
expect("@timestamp Trg-Done QS_RX_TICK")
