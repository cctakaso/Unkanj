# Unkanj Project readme
Copyright (c) 2025 Takashi Ogisawa
last update  2025/12/09

## 概要
Unkanjは、WindowsDLLライブラリプロジェクトです。このリポジトリには、アプリケーションのソースコード、リソース、および関連する設定ファイルが含まれています。

## 特徴・機能
Unkanjは現在以下の機能・特徴を有しています
1. テキストの漢字コード自動判別を行います。（JIS, EUC, Shift-JIS, UTF-8）
2. テキストの漢字コード相互変換を行います。（JIS, EUC, Shift-JIS, UTF-8）
3. コマンドファイル：unkanji.exeでは、オプション指示に従い動作します。
4. ダイナミックライブラリ：unkanji.dllは、API定義に従います。

それぞれの、使いかたの詳しくは、unkanji.txtおよびunkanji.hにあります。

## 関連プロジェクト
FileDiver, MintEditorプロジェクトでは、漢字処理にUnKanj.dllを使用しています。

## ライセンス
This project is licensed under the zlib License - see the [LICENSE](https://zlib.net/zlib_license.html) file for details.
