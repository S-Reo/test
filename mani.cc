#include <stdio.h>
#include <unistd.h>
//マニピュレータは/run/shm/anglesの中の値を読んで動作する
//電磁弁は、/run/shm/ev_on_off に1または0の出力で開閉できる
//アナログセンサの値は、/run/shm/adconv_values の中身を見ればよい。取得したタイミングで変わるので、どう取得するか。一定周期でファイルオープンしてreadして配列に突っ込むのがよさそう
//関節角をマニピュレータに送る関数
//引数: 各関節角とwait（ms）
void sendAngles(int j1,int j2,int j3,int j5,int j6,double wait)
{
	FILE *manip = fopen("/run/shm/angles","w");//各関節の角度を記述したいファイルを開く
	if(manip == NULL)
		return;
	fprintf(manip,"%d,%d,%d,%d,%d\n",j1,j2,j3,j5,j6);//指定したファイルに各関節の角度を出力
	fclose(manip);//ファイルを閉じる
	usleep((int)(wait)*1000);//待つ
	//ファイルに角度を出力することでマニピュレータがその角度になるように動く
}
//
//自分から見て右から順に並べていく
//先端の角度が30度を超えないようにしておく
//先端のz軸回転はさせない
//先端の高さをぎりぎりに調節する - chromeのGUIで調節する
//一直線に並べる。

//電磁弁を操作する関数
//
void changeValve(char change, double wait)
{
	FILE * valve = fopen("/run/shm/ev_on_off","w");
	if(valve == NULL)
		return;
	fprintf(valve, "%c\n", change);
	fclose(valve);
	usleep((int)(wait*1000));
	//引数に1か0を入れて開閉する
}

//とりあえずセンサをを使わずにやる
//本番の流れ
//1. ハンドを取り付ける前に、アームの初期位置を指定して動かしておく
//2. ハンドを取り付ける
//3. 何かしら支持を与えて動作開始
//4. 自分から見て一番右まで移動
//5. 電磁弁操作(開閉)
//6. 地面と水平に、ドミノと平行に引き抜く - 次におきたい場所への移動
//7. 一個並べられた。
//8. 5と6を繰り返す
//9.18回繰り返して終了
//

//初期位置を指定する関数
void initializePosture(double wait)
{
	sendAngles(-30, 103, 76, -89,0,1000/*初期姿勢をどこにするかGUIで確認*/);
	usleep((int)(wait*1000));
}
//最後に自分で突っついてドミノを倒す動作
//運動学を解かないと調整が面倒くさい
void layDomino's(int * last_angle)
{
	
	//J1だけ変数としているので、J1を使う。
	//j1そのままに、高さを20mm程上げて、手前か奥に5~10mm程平行移動。その後J1を数度+する。
		
 	sendAngles(last_angle, 103, 76, -89,0,1000/*初期姿勢をどこにするかGUIで確認*/);
	
	//matlabで行列を解けばいいんだっけ
	//腕の長さを取らないと運動学解けなくね
	//腕のデータが出ていないみたいなので、仕方なく決め打ち
}

//メイン処理
int main(int argc, char const* argv[])//この引数にはどこからデータを入れるのか
{
	initializePosture(1000);

//	changeValve('1',1000);

	changeValve('0',1000);

	int monuments_num=0;
	printf("繰り返し回数 : "); scanf("%d",&monuments_num);
	char exe='s';
	printf("開始します"); //scanf("%c",&exe);
//j1 = z軸
//j2 = 腰
//j3
//j4
//j5 の限界 = 
//j6 
//J を宣言
//

	int J = 50;
	if(exe != 's')
	{
		printf("終了\n");
	}	
	else
	{
		//開始位置へ移動
 	        sendAngles(J, 103, 76, -89,0,2000/*初期姿勢をどこにするかGUIで確認*/);

		for(int i=0; i < monuments_num; i++)
		{
			J -= 4;

			changeValve('1',1000);//ドミノを置く

			changeValve('0',1000);//シリンダを戻す
	                
 	        	sendAngles(J, 103, 76, -89,0,1000/*初期姿勢をどこにするかGUIで確認*/);
			//各引数を一定値ずつ変更する

		}

		lay_Domino's(J);
		
		initializePosture(1000);//元の位置に戻る
		printf("終了\n");
	
	}

/*	for(int i=0;i<10;i++){//2点を交互に行き来する
		if(i%2){
			sendAngles(45,45,45,45,45,3000.0);//移動後の角度を引数にいれてやる
		}else{
			sendAngles(0,0,0,0,0,3000.0);
		}
	}
*/

	return 0;
}
