#! /bin/bash
nohup ./chk_intmit.sh >& logintmit &
nohup ./chk_intheb1.sh >& logintheb1 &
nohup ./chk_intheb2.sh >& logintheb2 &
nohup ./chk_intheb3.sh >& logintheb3 &
nohup ./chk_intheb4.sh >& logintheb4 &
nohup ./chk_intblake1.sh >& logintblake1 &
nohup ./chk_indep >& logindep &
nohup ./chk_alg >& logalg &
nohup ./chk_exp >& logexp &
nohup ./chk_log >& loglog &
nohup ./chk_trig >& logtrig &
nohup ./chk_invtrig >& loginvtrig &
nohup ./chk_hyp >& loghyp &
nohup ./chk_invhyp >& loginvhyp &
nohup ./chk_spec >& logspec &
