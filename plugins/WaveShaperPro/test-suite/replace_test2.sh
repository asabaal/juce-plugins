sed -i '/test_clipping_behavior()/,/^}$/c\
test_clipping_behavior() {\
    echo "Test 2: Clipping behavior"\
    \
    # Test at extreme drive where hard clip should max out at 1.0\
    ../cli-processor/build/waveshaper_cli sine_440.wav temp_soft_90.wav --curve soft --drive 90\
    ../cli-processor/build/waveshaper_cli sine_440.wav temp_hard_90.wav --curve hard --drive 90\
    \
    peak_soft=$(sox temp_soft_90.wav -n stat 2>&1 | grep "Maximum amplitude" | awk '\''{print $3}'\'')\
    peak_hard=$(sox temp_hard_90.wav -n stat 2>&1 | grep "Maximum amplitude" | awk '\''{print $3}'\'')\
    \
    # Hard clip should max out at exactly 1.0, soft should be less\
    if (( $(echo "$peak_hard > $peak_soft" | bc -l) )) && (( $(echo "$peak_hard >= 0.999" | bc -l) )); then\
        echo "  ✅ Hard clip reaches limit better: Soft=$peak_soft, Hard=$peak_hard"\
    else\
        echo "  ❌ Hard clip should reach higher peaks: Soft=$peak_soft, Hard=$peak_hard"\
    fi\
    \
    rm -f temp_soft_90.wav temp_hard_90.wav\
}' validate_tests.sh
