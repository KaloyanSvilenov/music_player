{ stdenv, lib, qt6, wrapQtAppsHook }:

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

  configurePhase = ''
    qmake6 PREFIX=$out \
           CONFIG+=nix \
           music_player.pro
  '';

  installPhase = ''
    install -Dm755 music_player $out/bin/music_player
    mkdir -p $out/share/${pname}/resources
    cp -r resources/* $out/share/${pname}/resources/
    wrapQtApp $out/bin/music_player \
      --set QT_PLUGIN_PATH "${qt6.qtbase}/${qt6.qtbase.qtPluginPrefix}" \
      --prefix PATH : ${lib.makeBinPath [ qt6.qtbase ]}
  '';
}
