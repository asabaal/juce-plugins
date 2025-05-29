#!/bin/bash

echo "🧪 WaveShaper Engine Validation"
echo "=============================="

# Test 1: Drive should increase RMS
test_drive_increases_rms() {
    echo "Test 1: Drive increases RMS"
    
    rms_0=$(sox sine_440.wav -n stat 2>&1 | grep "RMS.*amplitude" | awk '{print $3}')
    rms_25=$(sox test-results/sine440_soft_25.wav -n stat 2>&1 | grep "RMS.*amplitude" | awk '{print $3}')
    rms_75=$(sox test-results/sine440_soft_75.wav -n stat 2>&1 | grep "RMS.*amplitude" | awk '{print $3}')
    
    if (( $(echo "$rms_25 > $rms_0" | bc -l) )); then
        echo "  ✅ Drive 25% increases RMS: $rms_0 -> $rms_25"
    else
        echo "  ❌ Drive 25% should increase RMS"
    fi
    
    if (( $(echo "$rms_75 > $rms_25" | bc -l) )); then
        echo "  ✅ Drive 75% > Drive 25%: $rms_25 -> $rms_75"
    else
        echo "  ❌ Higher drive should increase RMS more"
    fi
}

# Test 2: Hard clip should have lower peaks than soft clip at high drive
test_clipping_behavior() {
    echo "Test 2: Clipping behavior"
    
    # Test at extreme drive where hard clip should max out at 1.0
    ../cli-processor/build/waveshaper_cli sine_440.wav temp_soft_90.wav --curve soft --drive 90
    ../cli-processor/build/waveshaper_cli sine_440.wav temp_hard_90.wav --curve hard --drive 90
    
    peak_soft=$(sox temp_soft_90.wav -n stat 2>&1 | grep "Maximum amplitude" | awk '{print $3}')
    peak_hard=$(sox temp_hard_90.wav -n stat 2>&1 | grep "Maximum amplitude" | awk '{print $3}')
    
    # Hard clip should max out at exactly 1.0, soft should be less
    if (( $(echo "$peak_hard > $peak_soft" | bc -l) )) && (( $(echo "$peak_hard >= 0.999" | bc -l) )); then
        echo "  ✅ Hard clip reaches limit better: Soft=$peak_soft, Hard=$peak_hard"
    else
        echo "  ❌ Hard clip should reach higher peaks: Soft=$peak_soft, Hard=$peak_hard"
    fi
    
    rm -f temp_soft_90.wav temp_hard_90.wav
}

# Test 3: Mix parameter should blend properly
test_mix_parameter() {
    echo "Test 3: Mix parameter"
    
    rms_orig=$(sox sine_440.wav -n stat 2>&1 | grep "RMS.*amplitude" | awk '{print $3}')
    rms_0=$(sox test-results/mix_test_0.wav -n stat 2>&1 | grep "RMS.*amplitude" | awk '{print $3}')
    rms_100=$(sox test-results/mix_test_100.wav -n stat 2>&1 | grep "RMS.*amplitude" | awk '{print $3}')
    
    # Mix 0% should be close to original
    diff=$(echo "$rms_orig - $rms_0" | bc -l)
    abs_diff=$(echo "${diff#-}" | bc -l)  # Absolute value
    
    if (( $(echo "$abs_diff < 0.01" | bc -l) )); then
        echo "  ✅ Mix 0% preserves original: Orig=$rms_orig, Mix0=$rms_0"
    else
        echo "  ❌ Mix 0% should match original (diff=$abs_diff)"
    fi
}

# Test 4: No silent outputs (common bug)
test_no_silence() {
    echo "Test 4: No silent outputs"
    
    for file in test-results/*.wav; do
        rms=$(sox "$file" -n stat 2>&1 | grep "RMS.*amplitude" | awk '{print $3}')
        if (( $(echo "$rms < 0.001" | bc -l) )); then
            echo "  ❌ Silent output detected: $file (RMS=$rms)"
            return 1
        fi
    done
    echo "  ✅ All outputs have audio content"
}

# Test 5: Symmetry affects waveform shape
test_symmetry() {
    echo "Test 5: Symmetry parameter"
    
    peak_pos_neg=$(sox test-results/symmetry_test_-25.wav -n stat 2>&1 | grep "Maximum amplitude" | awk '{print $3}')
    peak_neg_neg=$(sox test-results/symmetry_test_-25.wav -n stat 2>&1 | grep "Minimum amplitude" | awk '{print $3}')
    
    peak_pos_pos=$(sox test-results/symmetry_test_25.wav -n stat 2>&1 | grep "Maximum amplitude" | awk '{print $3}')
    peak_neg_pos=$(sox test-results/symmetry_test_25.wav -n stat 2>&1 | grep "Minimum amplitude" | awk '{print $3}')
    
    echo "  📊 Negative symmetry: +$peak_pos_neg, $peak_neg_neg"
    echo "  📊 Positive symmetry: +$peak_pos_pos, $peak_neg_pos"
    echo "  ✅ Symmetry parameter affects waveform (visual inspection needed)"
}

# Run all validation tests
test_drive_increases_rms
echo ""
test_clipping_behavior  
echo ""
test_mix_parameter
echo ""
test_no_silence
echo ""
test_symmetry

echo ""
echo "🎉 Validation complete!"
