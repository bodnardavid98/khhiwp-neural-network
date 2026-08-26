# Build
git submodule update --init --recursive  
scons platform=linux api_version=4.7  

# Clean
scons -c api_version=4.7  
