{ stdenv
, lib
, qt6
, wrapQtAppsHook
}:

stdenv.mkDerivation rec {
  pname = "music-player";
  version = "0.1.0";

  src = ./.;

  nativeBuildInputs = [
    qt6.full
    qt6.wrapQtAppsHook
  ];

  buildInputs = [
    qt6.qtmultimedia
  ];

  # QMake specific settings
  dontUseCmakeConfigure = true;
  enableParallelBuilding = true;

  configurePhase = ''
    runHook preConfigure
    qmake6 PREFIX=$out \
      CONFIG+=release \
      QMAKE_CXXFLAGS+=" -std=c++17" \
      DESTDIR=release \
      OBJECTS_DIR=release/obj \
      MOC_DIR=build/moc \
      RCC_DIR=build/rcc \
      UI_DIR=build/ui \
      music_player.pro
    runHook postConfigure
  '';

  buildPhase = ''
    runHook preBuild
    make -j$NIX_BUILD_CORES
    runHook postBuild
  '';

  installPhase = ''
    runHook preInstall
    
    # Install the binary (it's built in the release directory)
    install -Dm755 release/music_player $out/bin/${pname}
    
    # Install resources
    mkdir -p $out/share/${pname}/resources
    cp -r resources/* $out/share/${pname}/resources/
    
    # Create Qt wrapper
    wrapQtApp $out/bin/${pname} \
      --set QT_PLUGIN_PATH "${qt6.qtbase}/${qt6.qtbase.qtPluginPrefix}" \
      --prefix QML2_IMPORT_PATH : "${qt6.qtbase}/${qt6.qtbase.qtQmlPrefix}"
    
    runHook postInstall
  '';
}
