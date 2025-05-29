#!/bin/bash

# WaveShaper Pro Test Suite
echo "🎵 WaveShaper Pro Engine Test Suite"
echo "=================================="

# Setup
CLI="../cli-processor/build/waveshaper_cli"
TEST_DIR="test-results"
mkdir -p $TEST_DIR

# Create test signals
echo "📊 Creating test signals..."
sox -n -r 44100 -b 16 sine_440.wav synth 2 sin 440
sox -n -r 44100 -b 16 sine_100.wav synth 2 sin 100  
sox -n -r 44100 -b 16 sine_1000.wav synth 2 sin 1000
sox -n -r 44100 -b 16 complex.wav synth 2 sin 440 sin 880 sin 1320  # Harmonics
sox -n -r 44100 -b 16 noise.wav synth 2 whitenoise vol 0.1

echo "✅ Test signals created"

# Test Suite Functions
run_curve_tests() {
    local input_file=$1
    local test_name=$2
    
    echo "🔧 Testing curves with $test_name..."
    
    # Test each curve type at different drive levels
    for curve in soft hard asymmetric tube; do
        for drive in 25 50 75 100; do
            output="$TEST_DIR/${test_name}_${curve}_${drive}.wav"
            $CLI $input_file $output --curve $curve --drive $drive
            
            # Analyze results
            stats=$(sox $output -n stat 2>&1)
            rms=$(echo "$stats" | grep "RMS.*amplitude" | awk '{print $3}')
            peak=$(echo "$stats" | grep "Maximum amplitude" | awk '{print $3}')
            
            echo "  $curve @ ${drive}%: Peak=$peak, RMS=$rms"
        done
    done
}

run_parameter_tests() {
    echo "⚙️  Testing parameter ranges..."
    
    # Mix parameter test
    for mix in 0 25 50 75 100; do
        output="$TEST_DIR/mix_test_${mix}.wav"
        $CLI sine_440.wav $output --curve hard --drive 75 --mix $mix
        
        stats=$(sox $output -n stat 2>&1)
        rms=$(echo "$stats" | grep "RMS.*amplitude" | awk '{print $3}')
        echo "  Mix ${mix}%: RMS=$rms"
    done
    
    # Symmetry parameter test  
    for sym in -50 -25 0 25 50; do
        output="$TEST_DIR/symmetry_test_${sym}.wav"
        $CLI sine_440.wav $output --curve asymmetric --drive 50 --symmetry $sym
        
        stats=$(sox $output -n stat 2>&1) 
        peak_pos=$(echo "$stats" | grep "Maximum amplitude" | awk '{print $3}')
        peak_neg=$(echo "$stats" | grep "Minimum amplitude" | awk '{print $3}')
        echo "  Symmetry ${sym}%: Peak+=$peak_pos, Peak-=$peak_neg"
    done
    
    # Gain tests
    for gain in -12 -6 0 6 12; do
        output="$TEST_DIR/gain_test_${gain}.wav"
        $CLI sine_440.wav $output --curve soft --drive 25 --input-gain $gain --output-gain $gain
        
        stats=$(sox $output -n stat 2>&1)
        rms=$(echo "$stats" | grep "RMS.*amplitude" | awk '{print $3}')
        echo "  Gain ${gain}dB: RMS=$rms"
    done
}

run_regression_tests() {
    echo "🔍 Running regression tests..."
    
    # Test that should produce predictable results
    $CLI sine_440.wav $TEST_DIR/regression_baseline.wav --curve soft --drive 0
    $CLI sine_440.wav $TEST_DIR/regression_identity.wav --curve soft --drive 0 --mix 0
    
    # Compare files (should be identical or nearly so)
    if cmp -s sine_440.wav $TEST_DIR/regression_identity.wav; then
        echo "  ✅ Identity test passed (0% mix = original)"
    else
        echo "  ❌ Identity test failed"
    fi
    
    # Test extremes don't crash
    $CLI sine_440.wav $TEST_DIR/extreme_drive.wav --curve hard --drive 100 2>/dev/null
    if [ $? -eq 0 ]; then
        echo "  ✅ Extreme drive test passed"
    else
        echo "  ❌ Extreme drive test failed"
    fi
}

run_performance_tests() {
    echo "⚡ Performance tests..."
    
    # Large file test
    sox -n -r 44100 -b 16 large_test.wav synth 30 sin 440  # 30 seconds
    
    start_time=$(date +%s.%N)
    $CLI large_test.wav $TEST_DIR/large_processed.wav --curve tube --drive 50
    end_time=$(date +%s.%N)
    
    duration=$(echo "$end_time - $start_time" | bc)
    echo "  Processed 30s file in ${duration}s"
    
    rm large_test.wav  # Clean up
}

# Run all tests
echo ""
run_curve_tests "sine_440.wav" "sine440"
echo ""
run_curve_tests "complex.wav" "complex" 
echo ""
run_parameter_tests
echo ""
run_regression_tests
echo ""
run_performance_tests

echo ""
echo "🎉 Test suite completed!"
echo "📁 Results saved in: $TEST_DIR/"
echo ""
echo "📈 Quick analysis:"
echo "   ls -lh $TEST_DIR/*.wav | head -10"
ls -lh $TEST_DIR/*.wav | head -10
