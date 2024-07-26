#!/bin/bash


test_file="sample.1"

cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef PRINT_DATE
Today is $(date)
#-- @endif
EOF

show_notice "Asserting sample.1.ok ..."

sampleargs="-DPRINT_DATE"
test_begin "$test_file" "Check X" "$sampleargs"

$spp $sampleargs $builddir/sample.1
if [ "$(wc -l < $builddir/sample.1.spp)" = "2" ]; then
    test_ok
else
    test_fail
fi

cat > $builddir/sample.1.ok <<EOF
#!/bin/bash
echo "ABC"
EOF

test_begin "$test_file" "Check '(X)'" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef (PRINT_DATE)
echo "ABC"
#-- @endif
EOF
run_local_sample_test $sampleargs sample.1


sampleargs="-DA_TRUE"
test_begin "$test_file" "Check '(X) || X'" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef (PRINT_DATE) || A_TRUE
echo "ABC"
#-- @endif
EOF
run_local_sample_test $sampleargs sample.1

sampleargs="-DA_TRUE -DPRINT_DATE"
test_begin "$test_file" "Check '(((X))) && X'" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef (((PRINT_DATE))) && A_TRUE
echo "ABC"
#-- @endif
EOF
run_local_sample_test "$sampleargs" sample.1

test_begin "$test_file" "Check '((( X )))'" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef (((  PRINT_DATE  )))
echo "ABC"
#-- @endif
EOF
run_local_sample_test "$sampleargs" sample.1

sampleargs="-DPRINT_DATE"
test_begin "$test_file" "Check 'X | ((( X )))'" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef A_TRUE || (((  PRINT_DATE  )))
echo "ABC"
#-- @endif
EOF
run_local_sample_test "$sampleargs" sample.1

sampleargs="-DPRINT_DATE -DA_FALSE"
test_begin "$test_file" "" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef A_TRUE || (((  PRINT_DATE  ))) && A_FALSE
echo "ABC"
#-- @endif
EOF
run_local_sample_test "$sampleargs" sample.1


sampleargs="-DPRINT_DATE"
test_begin "$test_file" "" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef (((PRINT_DATE  )))
echo "ABC"
#-- @elif PRINT_DATE
echo "DEF"
#-- @else
echo "XYZ"
#-- @endif
EOF
run_local_sample_test "$sampleargs" sample.1

sampleargs="-DPRINT_DATE"
test_begin "$test_file" "" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef (((PRINT_DATER  )))
echo "DEF"
#-- @elif PRINT_DATE
echo "ABC"
#-- @else
echo "XYZ"
#-- @endif
EOF
run_local_sample_test "$sampleargs" sample.1

sampleargs="-DAA -DAS"
test_begin "$test_file" "" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef (((PRINT_DATER  )))
echo "DEF"
#-- @elif AA && AS 
echo "ABC"
#-- @else
echo "XYZ"
#-- @endif
EOF
run_local_sample_test "$sampleargs" sample.1

sampleargs="-DAB -DAA -DAS"
test_begin "$test_file" "" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef (((PRINT_DATER  )))
echo "DEF"
#-- @elif AA && AS || AB
echo "ABC"
#-- @else
echo "XYZ"
#-- @endif
EOF
run_local_sample_test "$sampleargs" sample.1

test_begin "$test_file" "" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef (((PRINT_DATER  )))
echo "DEF"
#-- @elif AA && (AS || AB)
echo "ABC"
#-- @else
echo "XYZ"
#-- @endif
EOF
run_local_sample_test "$sampleargs" sample.1

sampleargs="-DAB -DAS"
test_begin "$test_file" "" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef (((PRINT_DATER  )))
echo "DEF"
#-- @elif AA && (AS || AB)
echo "XYZ"
#-- @else
echo "ABC"
#-- @endif
EOF
run_local_sample_test "$sampleargs" sample.1

sampleargs="-DAB -DSS"
test_begin "$test_file" "" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef (((PRINT_DATER  )))
echo "DEF"
#-- @elif AA && (AS || AB)
echo "XYZ"
#-- @elif AA && (AS || AB) && SS
echo "OPQ"
#-- @else
echo "ABC"
#-- @endif
EOF
run_local_sample_test "$sampleargs" sample.1

sampleargs="-DAB -DSS -DFF"
test_begin "$test_file" "" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef (((PRINT_DATER  )))
echo "DEF"
#-- @elif AA && (AS || AB)
echo "XYZ"
#-- @elif AA && (AS || AB) && SS
echo "OPQ"
#-- @else
#-- @ifdef BC
echo "CBA"
#-- @else
echo "ABC"
#-- @endif BC
#-- @endif
EOF
run_local_sample_test "$sampleargs" sample.1


sampleargs="-DAB -DSS -DFF"
test_begin "$test_file" "" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef (((PRINT_DATER  )))

#-- @elif AA && (AS || AB)
echo "XYZ"
#-- @elif AA && (AS || AB) && SS
echo "OPQ"
#-- @else
#-- @ifdef BC
echo "CBA"
#-- @else
echo "ABC"
#-- @endif BC
#-- @endif
EOF
run_local_sample_test "$sampleargs" sample.1

###########################

cat > $builddir/sample.1.ok <<EOF
#!/bin/bash
echo 1
EOF

sampleargs="-DAB -DSS -DFF -DAD"
test_begin "$builddir/sample.1" "" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef FF
#-- @ifdef AD
echo 1
#-- @else
echo 2
#-- @endif AD
#-- @elif AA && (AS || AB)
echo 3
#-- @elif AA && (AS || AB) && SS
echo 4
#-- @else
#-- @ifdef BC
echo 5
#-- @else
echo 6
#-- @endif BC
#-- @endif FF
EOF
run_local_sample_test "$sampleargs" sample.1


###################################

cat > $builddir/sample.1.ok <<EOF
#!/bin/bash
echo 1
echo 0
EOF

sampleargs="-DAB -DSS -DFF -DAD"
test_begin "$builddir/sample.1" "" "$sampleargs"
cat > $builddir/sample.1 <<EOF
#!/bin/bash
#-- @ifdef FF
#-- @ifdef A1
echo 01
#-- @elif A2
echo 02
#-- @elif AD
echo 1
#-- @endif A1
echo 0
#-- @elif AA && (AS || AB)
echo 3
#-- @elif AA && (AS || AB) && SS
echo 4
#-- @else
#-- @ifdef BC
echo 5
#-- @else
echo 6
#-- @endif BC
#-- @endif FF
EOF
run_local_sample_test "$sampleargs" sample.1