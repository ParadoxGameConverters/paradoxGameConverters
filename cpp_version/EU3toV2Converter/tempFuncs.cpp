#include "tempFuncs.h"

void outputHeader(FILE* output)
{
	fprintf(output, "date=\"1836.1.1\"\n");
	fprintf(output, "player=\"PRU\"\n");
	fprintf(output, "government=5\n");
	fprintf(output, "automate_trade=no\n");
	fprintf(output, "automate_sliders=0\n");
	fprintf(output, "rebel=3\n");
	fprintf(output, "unit=1895\n");
	fprintf(output, "state=1177\n");
	fprintf(output, "flags=\n");
	fprintf(output, "{\n");
	fprintf(output, "}\n");
	fprintf(output, "gameplaysettings=\n");
	fprintf(output, "{\n");
	fprintf(output, "	setgameplayoptions=\n");
	fprintf(output, "	{\n");
	fprintf(output, "2 	}\n");
	fprintf(output, "}\n");
	fprintf(output, "start_date=\"1836.1.1\"\n");
	fprintf(output, "start_pop_index=26489\n");
	fprintf(output, "worldmarket=\n");
	fprintf(output, "{\n");
	fprintf(output, "	worldmarket_pool=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=6.71817\n");
	fprintf(output, "		small_arms=1.08435\n");
	fprintf(output, "		artillery=3.49246\n");
	fprintf(output, "		canned_food=9.11725\n");
	fprintf(output, "		cotton=800.47363\n");
	fprintf(output, "		dye=354.33838\n");
	fprintf(output, "		wool=1984.03055\n");
	fprintf(output, "		silk=201.23349\n");
	fprintf(output, "		coal=1312.75400\n");
	fprintf(output, "		sulphur=117.55896\n");
	fprintf(output, "		iron=603.87610\n");
	fprintf(output, "		timber=1168.36642\n");
	fprintf(output, "		tropical_wood=300.83521\n");
	fprintf(output, "		precious_metal=15.45703\n");
	fprintf(output, "		steel=49.94543\n");
	fprintf(output, "		cement=3.51053\n");
	fprintf(output, "		machine_parts=10.49741\n");
	fprintf(output, "		glass=132.60635\n");
	fprintf(output, "		fertilizer=128.89102\n");
	fprintf(output, "		explosives=2.77228\n");
	fprintf(output, "		clipper_convoy=1.57492\n");
	fprintf(output, "		steamer_convoy=3.99484\n");
	fprintf(output, "		fabric=569.80630\n");
	fprintf(output, "		lumber=759.37601\n");
	fprintf(output, "		paper=86.62170\n");
	fprintf(output, "		cattle=684.57086\n");
	fprintf(output, "		fish=1808.87393\n");
	fprintf(output, "		fruit=2518.43427\n");
	fprintf(output, "		grain=3565.08829\n");
	fprintf(output, "		tobacco=1948.03577\n");
	fprintf(output, "		tea=3161.84256\n");
	fprintf(output, "		coffee=1357.97589\n");
	fprintf(output, "		opium=28.70587\n");
	fprintf(output, "		wine=25.95401\n");
	fprintf(output, "		liquor=146.89957\n");
	fprintf(output, "		regular_clothes=191.00803\n");
	fprintf(output, "		luxury_clothes=1.39642\n");
	fprintf(output, "		furniture=35.18484\n");
	fprintf(output, "		luxury_furniture=1.21490\n");
	fprintf(output, "	}\n");
	fprintf(output, "	price_pool=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=17.14966\n");
	fprintf(output, "		small_arms=37.00000\n");
	fprintf(output, "		artillery=60.00000\n");
	fprintf(output, "		canned_food=16.00000\n");
	fprintf(output, "		barrels=98.00000\n");
	fprintf(output, "		aeroplanes=110.00000\n");
	fprintf(output, "		cotton=2.00000\n");
	fprintf(output, "		dye=11.75977\n");
	fprintf(output, "		wool=0.68604\n");
	fprintf(output, "		silk=10.20020\n");
	fprintf(output, "		coal=2.29999\n");
	fprintf(output, "		sulphur=6.12012\n");
	fprintf(output, "		iron=3.42993\n");
	fprintf(output, "		timber=0.89999\n");
	fprintf(output, "		tropical_wood=5.45404\n");
	fprintf(output, "		rubber=7.00000\n");
	fprintf(output, "		oil=12.00000\n");
	fprintf(output, "		precious_metal=7.50000\n");
	fprintf(output, "		steel=4.79407\n");
	fprintf(output, "		cement=15.67969\n");
	fprintf(output, "		machine_parts=35.76929\n");
	fprintf(output, "		glass=2.95804\n");
	fprintf(output, "		fuel=25.00000\n");
	fprintf(output, "		fertilizer=10.20020\n");
	fprintf(output, "		explosives=20.20020\n");
	fprintf(output, "		clipper_convoy=42.00000\n");
	fprintf(output, "		steamer_convoy=63.69873\n");
	fprintf(output, "		electric_gear=16.00000\n");
	fprintf(output, "		fabric=1.60001\n");
	fprintf(output, "		lumber=1.02002\n");
	fprintf(output, "		paper=3.46805\n");
	fprintf(output, "		cattle=2.00000\n");
	fprintf(output, "		fish=1.46997\n");
	fprintf(output, "		fruit=1.76398\n");
	fprintf(output, "		grain=2.20001\n");
	fprintf(output, "		tobacco=1.10001\n");
	fprintf(output, "		tea=2.60001\n");
	fprintf(output, "		coffee=2.10001\n");
	fprintf(output, "		opium=3.23203\n");
	fprintf(output, "		automobiles=70.00000\n");
	fprintf(output, "		telephones=16.00000\n");
	fprintf(output, "		wine=9.89417\n");
	fprintf(output, "		liquor=6.52811\n");
	fprintf(output, "		regular_clothes=5.91608\n");
	fprintf(output, "		luxury_clothes=65.65063\n");
	fprintf(output, "		furniture=4.99808\n");
	fprintf(output, "		luxury_furniture=59.59058\n");
	fprintf(output, "		radio=16.00000\n");
	fprintf(output, "	}\n");
	fprintf(output, "	last_price_history=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=17.32483\n");
	fprintf(output, "		small_arms=36.62964\n");
	fprintf(output, "		artillery=59.39941\n");
	fprintf(output, "		canned_food=15.83984\n");
	fprintf(output, "		barrels=98.00000\n");
	fprintf(output, "		aeroplanes=110.00000\n");
	fprintf(output, "		cotton=2.00000\n");
	fprintf(output, "		dye=11.87988\n");
	fprintf(output, "		wool=0.69302\n");
	fprintf(output, "		silk=10.10010\n");
	fprintf(output, "		coal=2.29999\n");
	fprintf(output, "		sulphur=6.06006\n");
	fprintf(output, "		iron=3.46497\n");
	fprintf(output, "		timber=0.89999\n");
	fprintf(output, "		tropical_wood=5.39999\n");
	fprintf(output, "		rubber=7.00000\n");
	fprintf(output, "		oil=12.00000\n");
	fprintf(output, "		precious_metal=7.50000\n");
	fprintf(output, "		steel=4.74704\n");
	fprintf(output, "		cement=15.83984\n");
	fprintf(output, "		machine_parts=36.13464\n");
	fprintf(output, "		glass=2.92902\n");
	fprintf(output, "		fuel=25.00000\n");
	fprintf(output, "		fertilizer=10.10010\n");
	fprintf(output, "		explosives=20.20020\n");
	fprintf(output, "		clipper_convoy=41.57959\n");
	fprintf(output, "		steamer_convoy=64.34937\n");
	fprintf(output, "		electric_gear=16.00000\n");
	fprintf(output, "		fabric=1.60001\n");
	fprintf(output, "		lumber=1.01001\n");
	fprintf(output, "		paper=3.43402\n");
	fprintf(output, "		cattle=2.00000\n");
	fprintf(output, "		fish=1.48499\n");
	fprintf(output, "		fruit=1.78198\n");
	fprintf(output, "		grain=2.20001\n");
	fprintf(output, "		tobacco=1.10001\n");
	fprintf(output, "		tea=2.60001\n");
	fprintf(output, "		coffee=2.10001\n");
	fprintf(output, "		opium=3.20001\n");
	fprintf(output, "		automobiles=70.00000\n");
	fprintf(output, "		telephones=16.00000\n");
	fprintf(output, "		wine=9.79709\n");
	fprintf(output, "		liquor=6.46405\n");
	fprintf(output, "		regular_clothes=5.85803\n");
	fprintf(output, "		luxury_clothes=65.00000\n");
	fprintf(output, "		furniture=4.94904\n");
	fprintf(output, "		luxury_furniture=59.00000\n");
	fprintf(output, "		radio=16.00000\n");
	fprintf(output, "	}\n");
	fprintf(output, "	supply_pool=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=9.77875\n");
	fprintf(output, "		small_arms=3.08157\n");
	fprintf(output, "		artillery=5.96939\n");
	fprintf(output, "		canned_food=15.53738\n");
	fprintf(output, "		barrels=1.00000\n");
	fprintf(output, "		aeroplanes=1.00000\n");
	fprintf(output, "		cotton=1920.61371\n");
	fprintf(output, "		dye=387.81024\n");
	fprintf(output, "		wool=2653.82913\n");
	fprintf(output, "		silk=537.04288\n");
	fprintf(output, "		coal=3704.13052\n");
	fprintf(output, "		sulphur=231.15594\n");
	fprintf(output, "		iron=1261.79257\n");
	fprintf(output, "		timber=2491.39914\n");
	fprintf(output, "		tropical_wood=571.31781\n");
	fprintf(output, "		rubber=1.00000\n");
	fprintf(output, "		oil=1.00000\n");
	fprintf(output, "		precious_metal=15.45703\n");
	fprintf(output, "		steel=881.18298\n");
	fprintf(output, "		cement=18.12048\n");
	fprintf(output, "		machine_parts=10.75476\n");
	fprintf(output, "		glass=773.25369\n");
	fprintf(output, "		fuel=1.00000\n");
	fprintf(output, "		fertilizer=442.91904\n");
	fprintf(output, "		explosives=6.18524\n");
	fprintf(output, "		clipper_convoy=10.84335\n");
	fprintf(output, "		steamer_convoy=6.47104\n");
	fprintf(output, "		electric_gear=1.00000\n");
	fprintf(output, "		fabric=2601.11707\n");
	fprintf(output, "		lumber=1372.68015\n");
	fprintf(output, "		paper=951.85193\n");
	fprintf(output, "		cattle=1175.13382\n");
	fprintf(output, "		fish=2531.64243\n");
	fprintf(output, "		fruit=3883.84756\n");
	fprintf(output, "		grain=6463.66568\n");
	fprintf(output, "		tobacco=2458.90677\n");
	fprintf(output, "		tea=5027.21396\n");
	fprintf(output, "		coffee=1419.66705\n");
	fprintf(output, "		opium=1061.43207\n");
	fprintf(output, "		automobiles=1.00000\n");
	fprintf(output, "		telephones=1.00000\n");
	fprintf(output, "		wine=298.37234\n");
	fprintf(output, "		liquor=509.57144\n");
	fprintf(output, "		regular_clothes=1487.10764\n");
	fprintf(output, "		luxury_clothes=11.38129\n");
	fprintf(output, "		furniture=576.39786\n");
	fprintf(output, "		luxury_furniture=11.04282\n");
	fprintf(output, "		radio=1.00000\n");
	fprintf(output, "	}\n");
	fprintf(output, "	price_history=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=17.32483\n");
	fprintf(output, "		small_arms=36.62964\n");
	fprintf(output, "		artillery=59.39941\n");
	fprintf(output, "		canned_food=15.83984\n");
	fprintf(output, "		barrels=98.00000\n");
	fprintf(output, "		aeroplanes=110.00000\n");
	fprintf(output, "		cotton=2.00000\n");
	fprintf(output, "		dye=11.87988\n");
	fprintf(output, "		wool=0.69302\n");
	fprintf(output, "		silk=10.10010\n");
	fprintf(output, "		coal=2.29999\n");
	fprintf(output, "		sulphur=6.06006\n");
	fprintf(output, "		iron=3.46497\n");
	fprintf(output, "		timber=0.89999\n");
	fprintf(output, "		tropical_wood=5.39999\n");
	fprintf(output, "		rubber=7.00000\n");
	fprintf(output, "		oil=12.00000\n");
	fprintf(output, "		precious_metal=7.50000\n");
	fprintf(output, "		steel=4.74704\n");
	fprintf(output, "		cement=15.83984\n");
	fprintf(output, "		machine_parts=36.13464\n");
	fprintf(output, "		glass=2.92902\n");
	fprintf(output, "		fuel=25.00000\n");
	fprintf(output, "		fertilizer=10.10010\n");
	fprintf(output, "		explosives=20.20020\n");
	fprintf(output, "		clipper_convoy=41.57959\n");
	fprintf(output, "		steamer_convoy=64.34937\n");
	fprintf(output, "		electric_gear=16.00000\n");
	fprintf(output, "		fabric=1.60001\n");
	fprintf(output, "		lumber=1.01001\n");
	fprintf(output, "		paper=3.43402\n");
	fprintf(output, "		cattle=2.00000\n");
	fprintf(output, "		fish=1.48499\n");
	fprintf(output, "		fruit=1.78198\n");
	fprintf(output, "		grain=2.20001\n");
	fprintf(output, "		tobacco=1.10001\n");
	fprintf(output, "		tea=2.60001\n");
	fprintf(output, "		coffee=2.10001\n");
	fprintf(output, "		opium=3.20001\n");
	fprintf(output, "		automobiles=70.00000\n");
	fprintf(output, "		telephones=16.00000\n");
	fprintf(output, "		wine=9.79709\n");
	fprintf(output, "		liquor=6.46405\n");
	fprintf(output, "		regular_clothes=5.85803\n");
	fprintf(output, "		luxury_clothes=65.00000\n");
	fprintf(output, "		furniture=4.94904\n");
	fprintf(output, "		luxury_furniture=59.00000\n");
	fprintf(output, "		radio=16.00000\n");
	fprintf(output, "	}\n");
	fprintf(output, "	price_history=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=17.14966\n");
	fprintf(output, "		small_arms=37.00000\n");
	fprintf(output, "		artillery=60.00000\n");
	fprintf(output, "		canned_food=16.00000\n");
	fprintf(output, "		barrels=98.00000\n");
	fprintf(output, "		aeroplanes=110.00000\n");
	fprintf(output, "		cotton=2.00000\n");
	fprintf(output, "		dye=11.75977\n");
	fprintf(output, "		wool=0.68604\n");
	fprintf(output, "		silk=10.20020\n");
	fprintf(output, "		coal=2.29999\n");
	fprintf(output, "		sulphur=6.12012\n");
	fprintf(output, "		iron=3.42993\n");
	fprintf(output, "		timber=0.89999\n");
	fprintf(output, "		tropical_wood=5.45404\n");
	fprintf(output, "		rubber=7.00000\n");
	fprintf(output, "		oil=12.00000\n");
	fprintf(output, "		precious_metal=7.50000\n");
	fprintf(output, "		steel=4.79407\n");
	fprintf(output, "		cement=15.67969\n");
	fprintf(output, "		machine_parts=35.76929\n");
	fprintf(output, "		glass=2.95804\n");
	fprintf(output, "		fuel=25.00000\n");
	fprintf(output, "		fertilizer=10.20020\n");
	fprintf(output, "		explosives=20.20020\n");
	fprintf(output, "		clipper_convoy=42.00000\n");
	fprintf(output, "		steamer_convoy=63.69873\n");
	fprintf(output, "		electric_gear=16.00000\n");
	fprintf(output, "		fabric=1.60001\n");
	fprintf(output, "		lumber=1.02002\n");
	fprintf(output, "		paper=3.46805\n");
	fprintf(output, "		cattle=2.00000\n");
	fprintf(output, "		fish=1.46997\n");
	fprintf(output, "		fruit=1.76398\n");
	fprintf(output, "		grain=2.20001\n");
	fprintf(output, "		tobacco=1.10001\n");
	fprintf(output, "		tea=2.60001\n");
	fprintf(output, "		coffee=2.10001\n");
	fprintf(output, "		opium=3.23203\n");
	fprintf(output, "		automobiles=70.00000\n");
	fprintf(output, "		telephones=16.00000\n");
	fprintf(output, "		wine=9.89417\n");
	fprintf(output, "		liquor=6.52811\n");
	fprintf(output, "		regular_clothes=5.91608\n");
	fprintf(output, "		luxury_clothes=65.65063\n");
	fprintf(output, "		furniture=4.99808\n");
	fprintf(output, "		luxury_furniture=59.59058\n");
	fprintf(output, "		radio=16.00000\n");
	fprintf(output, "	}\n");
	fprintf(output, "	price_change=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=-0.17517\n");
	fprintf(output, "		small_arms=0.37036\n");
	fprintf(output, "		artillery=0.60059\n");
	fprintf(output, "		canned_food=0.16016\n");
	fprintf(output, "		dye=-0.12012\n");
	fprintf(output, "		wool=-0.00699\n");
	fprintf(output, "		silk=0.10010\n");
	fprintf(output, "		sulphur=0.06006\n");
	fprintf(output, "		iron=-0.03503\n");
	fprintf(output, "		tropical_wood=0.05405\n");
	fprintf(output, "		steel=0.04703\n");
	fprintf(output, "		cement=-0.16016\n");
	fprintf(output, "		machine_parts=-0.36536\n");
	fprintf(output, "		glass=0.02902\n");
	fprintf(output, "		fertilizer=0.10010\n");
	fprintf(output, "		clipper_convoy=0.42041\n");
	fprintf(output, "		steamer_convoy=-0.65063\n");
	fprintf(output, "		lumber=0.01001\n");
	fprintf(output, "		paper=0.03403\n");
	fprintf(output, "		fish=-0.01501\n");
	fprintf(output, "		fruit=-0.01801\n");
	fprintf(output, "		opium=0.03201\n");
	fprintf(output, "		wine=0.09708\n");
	fprintf(output, "		liquor=0.06406\n");
	fprintf(output, "		regular_clothes=0.05804\n");
	fprintf(output, "		luxury_clothes=0.65063\n");
	fprintf(output, "		furniture=0.04904\n");
	fprintf(output, "		luxury_furniture=0.59058\n");
	fprintf(output, "	}\n");
	fprintf(output, "	discovered_goods=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=1.00000\n");
	fprintf(output, "		small_arms=1.00000\n");
	fprintf(output, "		artillery=1.00000\n");
	fprintf(output, "		canned_food=1.00000\n");
	fprintf(output, "		cotton=1.00000\n");
	fprintf(output, "		dye=1.00000\n");
	fprintf(output, "		wool=1.00000\n");
	fprintf(output, "		silk=1.00000\n");
	fprintf(output, "		coal=1.00000\n");
	fprintf(output, "		sulphur=1.00000\n");
	fprintf(output, "		iron=1.00000\n");
	fprintf(output, "		timber=1.00000\n");
	fprintf(output, "		tropical_wood=1.00000\n");
	fprintf(output, "		rubber=1.00000\n");
	fprintf(output, "		oil=1.00000\n");
	fprintf(output, "		precious_metal=1.00000\n");
	fprintf(output, "		steel=1.00000\n");
	fprintf(output, "		cement=1.00000\n");
	fprintf(output, "		machine_parts=1.00000\n");
	fprintf(output, "		glass=1.00000\n");
	fprintf(output, "		fertilizer=1.00000\n");
	fprintf(output, "		explosives=1.00000\n");
	fprintf(output, "		clipper_convoy=1.00000\n");
	fprintf(output, "		steamer_convoy=1.00000\n");
	fprintf(output, "		fabric=1.00000\n");
	fprintf(output, "		lumber=1.00000\n");
	fprintf(output, "		paper=1.00000\n");
	fprintf(output, "		cattle=1.00000\n");
	fprintf(output, "		fish=1.00000\n");
	fprintf(output, "		fruit=1.00000\n");
	fprintf(output, "		grain=1.00000\n");
	fprintf(output, "		tobacco=1.00000\n");
	fprintf(output, "		tea=1.00000\n");
	fprintf(output, "		coffee=1.00000\n");
	fprintf(output, "		opium=1.00000\n");
	fprintf(output, "		wine=1.00000\n");
	fprintf(output, "		liquor=1.00000\n");
	fprintf(output, "		regular_clothes=1.00000\n");
	fprintf(output, "		luxury_clothes=1.00000\n");
	fprintf(output, "		furniture=1.00000\n");
	fprintf(output, "		luxury_furniture=1.00000\n");
	fprintf(output, "	}\n");
	fprintf(output, "	actual_sold=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=3.06058\n");
	fprintf(output, "		small_arms=1.99722\n");
	fprintf(output, "		artillery=2.47693\n");
	fprintf(output, "		canned_food=6.42014\n");
	fprintf(output, "		cotton=1120.14008\n");
	fprintf(output, "		dye=33.47186\n");
	fprintf(output, "		wool=669.79858\n");
	fprintf(output, "		silk=335.80939\n");
	fprintf(output, "		coal=2391.37653\n");
	fprintf(output, "		sulphur=113.59698\n");
	fprintf(output, "		iron=657.91647\n");
	fprintf(output, "		timber=1323.03271\n");
	fprintf(output, "		tropical_wood=270.48260\n");
	fprintf(output, "		steel=831.23755\n");
	fprintf(output, "		cement=14.60995\n");
	fprintf(output, "		machine_parts=0.25735\n");
	fprintf(output, "		glass=640.64734\n");
	fprintf(output, "		fertilizer=314.02802\n");
	fprintf(output, "		explosives=3.41296\n");
	fprintf(output, "		clipper_convoy=9.26843\n");
	fprintf(output, "		steamer_convoy=2.47620\n");
	fprintf(output, "		fabric=2031.31076\n");
	fprintf(output, "		lumber=613.30414\n");
	fprintf(output, "		paper=865.23022\n");
	fprintf(output, "		cattle=490.56296\n");
	fprintf(output, "		fish=722.76849\n");
	fprintf(output, "		fruit=1365.41330\n");
	fprintf(output, "		grain=2898.57739\n");
	fprintf(output, "		tobacco=510.87100\n");
	fprintf(output, "		tea=1865.37140\n");
	fprintf(output, "		coffee=61.69116\n");
	fprintf(output, "		opium=1032.72620\n");
	fprintf(output, "		wine=272.41833\n");
	fprintf(output, "		liquor=362.67188\n");
	fprintf(output, "		regular_clothes=1296.09961\n");
	fprintf(output, "		luxury_clothes=9.98486\n");
	fprintf(output, "		furniture=541.21301\n");
	fprintf(output, "		luxury_furniture=9.82791\n");
	fprintf(output, "	}\n");
	fprintf(output, "	actual_sold_world=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=1.29520\n");
	fprintf(output, "		small_arms=1.14532\n");
	fprintf(output, "		artillery=1.56018\n");
	fprintf(output, "		canned_food=9.80173\n");
	fprintf(output, "		barrels=1.00000\n");
	fprintf(output, "		aeroplanes=1.00000\n");
	fprintf(output, "		cotton=512.67557\n");
	fprintf(output, "		dye=178.57416\n");
	fprintf(output, "		wool=889.23181\n");
	fprintf(output, "		silk=1.42001\n");
	fprintf(output, "		coal=844.76251\n");
	fprintf(output, "		sulphur=112.42844\n");
	fprintf(output, "		iron=34.60110\n");
	fprintf(output, "		timber=475.57938\n");
	fprintf(output, "		tropical_wood=123.75558\n");
	fprintf(output, "		rubber=1.00000\n");
	fprintf(output, "		oil=1.00000\n");
	fprintf(output, "		precious_metal=1.00000\n");
	fprintf(output, "		steel=21.08966\n");
	fprintf(output, "		cement=8.86365\n");
	fprintf(output, "		machine_parts=4.22531\n");
	fprintf(output, "		glass=115.54666\n");
	fprintf(output, "		fuel=1.00000\n");
	fprintf(output, "		fertilizer=18.79861\n");
	fprintf(output, "		explosives=3.27560\n");
	fprintf(output, "		clipper_convoy=1.20129\n");
	fprintf(output, "		steamer_convoy=1.35526\n");
	fprintf(output, "		electric_gear=1.00000\n");
	fprintf(output, "		fabric=305.57602\n");
	fprintf(output, "		lumber=759.37683\n");
	fprintf(output, "		paper=18.22476\n");
	fprintf(output, "		cattle=460.85583\n");
	fprintf(output, "		fish=793.83334\n");
	fprintf(output, "		fruit=771.43027\n");
	fprintf(output, "		grain=1155.21808\n");
	fprintf(output, "		tobacco=912.65756\n");
	fprintf(output, "		tea=1077.08658\n");
	fprintf(output, "		coffee=721.61832\n");
	fprintf(output, "		opium=7.48077\n");
	fprintf(output, "		automobiles=1.00000\n");
	fprintf(output, "		telephones=1.00000\n");
	fprintf(output, "		wine=3.50369\n");
	fprintf(output, "		liquor=129.41281\n");
	fprintf(output, "		regular_clothes=39.22079\n");
	fprintf(output, "		luxury_clothes=3.10498\n");
	fprintf(output, "		furniture=4.96222\n");
	fprintf(output, "		luxury_furniture=1.15799\n");
	fprintf(output, "		radio=1.00000\n");
	fprintf(output, "	}\n");
	fprintf(output, "	real_demand=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=4.35577\n");
	fprintf(output, "		small_arms=5.31036\n");
	fprintf(output, "		artillery=40.41138\n");
	fprintf(output, "		canned_food=28.06891\n");
	fprintf(output, "		barrels=1.00000\n");
	fprintf(output, "		aeroplanes=1.00000\n");
	fprintf(output, "		cotton=1644.65237\n");
	fprintf(output, "		dye=212.04602\n");
	fprintf(output, "		wool=1559.03040\n");
	fprintf(output, "		silk=1032.11151\n");
	fprintf(output, "		coal=3274.68475\n");
	fprintf(output, "		sulphur=315.76611\n");
	fprintf(output, "		iron=692.51758\n");
	fprintf(output, "		timber=1974.24130\n");
	fprintf(output, "		tropical_wood=618.28745\n");
	fprintf(output, "		rubber=1.00000\n");
	fprintf(output, "		oil=1.00000\n");
	fprintf(output, "		precious_metal=1.00000\n");
	fprintf(output, "		steel=1684.51114\n");
	fprintf(output, "		cement=45.02441\n");
	fprintf(output, "		machine_parts=4.48267\n");
	fprintf(output, "		glass=1126.26407\n");
	fprintf(output, "		fuel=1.00000\n");
	fprintf(output, "		fertilizer=850.26920\n");
	fprintf(output, "		explosives=6.68857\n");
	fprintf(output, "		clipper_convoy=73.87799\n");
	fprintf(output, "		steamer_convoy=3.83145\n");
	fprintf(output, "		electric_gear=1.00000\n");
	fprintf(output, "		fabric=2434.83481\n");
	fprintf(output, "		lumber=1755.60486\n");
	fprintf(output, "		paper=3222.34103\n");
	fprintf(output, "		cattle=1033.13364\n");
	fprintf(output, "		fish=1516.60184\n");
	fprintf(output, "		fruit=2136.84357\n");
	fprintf(output, "		grain=4053.79547\n");
	fprintf(output, "		tobacco=1423.52856\n");
	fprintf(output, "		tea=2942.45798\n");
	fprintf(output, "		coffee=783.30948\n");
	fprintf(output, "		opium=1265.99176\n");
	fprintf(output, "		automobiles=1.00000\n");
	fprintf(output, "		telephones=1.00000\n");
	fprintf(output, "		wine=3224.02264\n");
	fprintf(output, "		liquor=945.38971\n");
	fprintf(output, "		regular_clothes=5164.17239\n");
	fprintf(output, "		luxury_clothes=17.78992\n");
	fprintf(output, "		furniture=2162.99100\n");
	fprintf(output, "		luxury_furniture=17.69485\n");
	fprintf(output, "		radio=1.00000\n");
	fprintf(output, "	}\n");
	fprintf(output, "	demand=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=15.13940\n");
	fprintf(output, "		small_arms=5.31036\n");
	fprintf(output, "		artillery=40.41138\n");
	fprintf(output, "		canned_food=28.06891\n");
	fprintf(output, "		barrels=1.00000\n");
	fprintf(output, "		aeroplanes=1.00000\n");
	fprintf(output, "		cotton=1652.32904\n");
	fprintf(output, "		dye=213.05737\n");
	fprintf(output, "		wool=1746.33661\n");
	fprintf(output, "		silk=24973.51263\n");
	fprintf(output, "		coal=4108.79709\n");
	fprintf(output, "		sulphur=337.91965\n");
	fprintf(output, "		iron=733.47171\n");
	fprintf(output, "		timber=2415.60379\n");
	fprintf(output, "		tropical_wood=14222.54645\n");
	fprintf(output, "		rubber=1.00000\n");
	fprintf(output, "		oil=1.00000\n");
	fprintf(output, "		precious_metal=1.00000\n");
	fprintf(output, "		steel=1962.08310\n");
	fprintf(output, "		cement=46.41095\n");
	fprintf(output, "		machine_parts=4.50949\n");
	fprintf(output, "		glass=1343.50537\n");
	fprintf(output, "		fuel=1.00000\n");
	fprintf(output, "		fertilizer=21613.47479\n");
	fprintf(output, "		explosives=17.62869\n");
	fprintf(output, "		clipper_convoy=73.87799\n");
	fprintf(output, "		steamer_convoy=3.83145\n");
	fprintf(output, "		electric_gear=1.00000\n");
	fprintf(output, "		fabric=2833.71994\n");
	fprintf(output, "		lumber=2187.99557\n");
	fprintf(output, "		paper=41348.17654\n");
	fprintf(output, "		cattle=1097.26886\n");
	fprintf(output, "		fish=1703.46161\n");
	fprintf(output, "		fruit=2341.28787\n");
	fprintf(output, "		grain=4497.72403\n");
	fprintf(output, "		tobacco=1820.38086\n");
	fprintf(output, "		tea=3677.57895\n");
	fprintf(output, "		coffee=1007.37848\n");
	fprintf(output, "		opium=31139.57016\n");
	fprintf(output, "		automobiles=1.00000\n");
	fprintf(output, "		telephones=1.00000\n");
	fprintf(output, "		wine=41349.85815\n");
	fprintf(output, "		liquor=1162.47229\n");
	fprintf(output, "		regular_clothes=66605.13165\n");
	fprintf(output, "		luxury_clothes=2597.96741\n");
	fprintf(output, "		furniture=22825.97485\n");
	fprintf(output, "		luxury_furniture=2597.87234\n");
	fprintf(output, "		radio=1.00000\n");
	fprintf(output, "	}\n");
	fprintf(output, "	player_balance=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=1.15204\n");
	fprintf(output, "		small_arms=-0.04266\n");
	fprintf(output, "		artillery=0.13162\n");
	fprintf(output, "		canned_food=-2.09070\n");
	fprintf(output, "		wool=-28.15579\n");
	fprintf(output, "		coal=456.76044\n");
	fprintf(output, "		sulphur=-4.89044\n");
	fprintf(output, "		iron=-4.02524\n");
	fprintf(output, "		timber=-4.13165\n");
	fprintf(output, "		steel=0.00003\n");
	fprintf(output, "		cement=-0.90942\n");
	fprintf(output, "		machine_parts=-4.21899\n");
	fprintf(output, "		glass=-11.82352\n");
	fprintf(output, "		fertilizer=-3.52103\n");
	fprintf(output, "		explosives=0.51013\n");
	fprintf(output, "		clipper_convoy=-0.08450\n");
	fprintf(output, "		fabric=-18.46896\n");
	fprintf(output, "		paper=-0.14139\n");
	fprintf(output, "		cattle=86.95898\n");
	fprintf(output, "		fish=34.94812\n");
	fprintf(output, "		fruit=143.36377\n");
	fprintf(output, "		grain=77.81180\n");
	fprintf(output, "		tobacco=-29.50455\n");
	fprintf(output, "		tea=-59.49030\n");
	fprintf(output, "		coffee=-17.00790\n");
	fprintf(output, "		opium=-0.00009\n");
	fprintf(output, "		wine=-0.00037\n");
	fprintf(output, "		liquor=-13.67694\n");
	fprintf(output, "		regular_clothes=-3.89795\n");
	fprintf(output, "		luxury_clothes=-0.00186\n");
	fprintf(output, "		furniture=-0.00043\n");
	fprintf(output, "		luxury_furniture=-0.00070\n");
	fprintf(output, "	}\n");
	fprintf(output, "}\n");
	fprintf(output, "overseas_penalty=\n");
	fprintf(output, "{\n");
	fprintf(output, "}\n");
	fprintf(output, "unit_cost=\n");
	fprintf(output, "{\n");
	fprintf(output, "	small_arms=0.41022\n");
	fprintf(output, "	artillery=0.02618\n");
	fprintf(output, "	canned_food=2.09906\n");
	fprintf(output, "	clipper_convoy=0.05035\n");
	fprintf(output, "	wine=0.03665\n");
	fprintf(output, "	liquor=0.01715\n");
	fprintf(output, "}\n");
	fprintf(output, "budget_balance=\n");
	fprintf(output, "{\n");
	fprintf(output, "	186.76614 }\n");
	fprintf(output, "fascist=\n");
	fprintf(output, "{\n");
	fprintf(output, "}\n");
	fprintf(output, "socialist=\n");
	fprintf(output, "{\n");
	fprintf(output, "}\n");
	fprintf(output, "communist=\n");
	fprintf(output, "{\n");
	fprintf(output, "}\n");
	fprintf(output, "anarcho_liberal=\n");
	fprintf(output, "{\n");
	fprintf(output, "}\n");
	fprintf(output, "canals=\n");
	fprintf(output, "{\n");
	fprintf(output, "0 0 0 0 }\n");
	fprintf(output, "id=\n");
	fprintf(output, "{\n");
	fprintf(output, "	id=2107\n");
	fprintf(output, "	type=4713\n");
	fprintf(output, "}\n");
	fprintf(output, "fired_events=\n");
	fprintf(output, "{\n");
	fprintf(output, "}\n");
}

void outputProvStuff(FILE* output)
{
	fprintf(output, "	rgo=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		employment=\n");
	fprintf(output, "		{\n");
	/*fprintf(output, "			province_id=1\n");
	fprintf(output, "			employees=\n");
	fprintf(output, "			{\n");
	fprintf(output, "			}\n");*/
	fprintf(output, "		}\n");
	//fprintf(output, "		last_income=75.92773\n");
	fprintf(output, "		goods_type=\"timber\"\n");
	fprintf(output, "	}\n");
	fprintf(output, "	life_rating=20\n");
	fprintf(output, "	last_imigration=\"1836.1.1\"\n");
}


void outputCountryHeader(FILE* output)
{
	fprintf(output, "	flags=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "	variables=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "	capital=300\n");
	fprintf(output, "	research_points=13.130\n");
	fprintf(output, "	technology=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "	research=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "	last_election=\"1836.1.1\"\n");
	fprintf(output, "	wage_reform=no_minimum_wage\n");
	fprintf(output, "	work_hours=no_work_hour_limit\n");
	fprintf(output, "	safety_regulations=trinket_safety\n");
	fprintf(output, "	unemployment_subsidies=trinket_subsidies\n");
	fprintf(output, "	pensions=no_pensions\n");
	fprintf(output, "	health_care=trinket_health_care\n");
	fprintf(output, "	slavery=no_slavery\n");
	fprintf(output, "	vote_franschise=wealth_voting\n");
	fprintf(output, "	upper_house_composition=appointed\n");
	fprintf(output, "	voting_system=first_past_the_post\n");
	fprintf(output, "	public_meetings=yes_meeting\n");
	fprintf(output, "	press_rights=censored_press\n");
	fprintf(output, "	trade_unions=non_socialist\n");
	fprintf(output, "	political_parties=non_secret_ballots\n");
	fprintf(output, "	upper_house=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		reactionary=0.04999\n");
	fprintf(output, "		conservative=0.82999\n");
	fprintf(output, "		liberal=0.12000\n");
	fprintf(output, "	}\n");
	fprintf(output, "	ruling_party=1\n");
	fprintf(output, "	active_party=1\n");
	fprintf(output, "	active_party=2\n");
	fprintf(output, "	active_party=5\n");
	fprintf(output, "	naval_need=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		clipper_convoy=2.04575\n");
	fprintf(output, "		steamer_convoy=2.04575\n");
	fprintf(output, "	}\n");
	fprintf(output, "	supply_cost=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		small_arms=0.27832\n");
	fprintf(output, "		artillery=8.76678\n");
	fprintf(output, "		canned_food=1.68106\n");
	fprintf(output, "		clipper_convoy=15.12146\n");
	fprintf(output, "		steamer_convoy=0.28076\n");
	fprintf(output, "		wine=0.02930\n");
	fprintf(output, "	}\n");
	fprintf(output, "	diplomatic_points=5.400\n");
	fprintf(output, "	religion=\"protestant\"\n");
	fprintf(output, "	government=hms_government\n");
	fprintf(output, "	plurality=5.029\n");
	fprintf(output, "	revanchism=0.000\n");
	fprintf(output, "	rich_tax=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		current=0.00003\n");
	fprintf(output, "		tax_income=\n");
	fprintf(output, "		{\n");
	fprintf(output, "0.00000 282668.31470 0.00000 0.00000 152561.20709 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 		}\n");
	fprintf(output, "		tax_eff=\n");
	fprintf(output, "		{\n");
	fprintf(output, "0.00000 688123.59158 0.00000 0.00000 430797.42029 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 		}\n");
	fprintf(output, "		total=1118921.01187\n");
	fprintf(output, "		rangeLimitMax=0.50000\n");
	fprintf(output, "		rangeLimitMin=0.00000\n");
	fprintf(output, "		max_tax=22\n");
	fprintf(output, "		min_tax=21\n");
	fprintf(output, "	}\n");
	fprintf(output, "	middle_tax=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		current=0.06256\n");
	fprintf(output, "		tax_income=\n");
	fprintf(output, "		{\n");
	fprintf(output, "0.00000 0.00000 725076.43088 5609.40536 0.00000 13224.41605 19321.00604 0.00000 0.00000 0.00000 1179.08530 0.00000 0.00000 		}\n");
	fprintf(output, "		tax_eff=\n");
	fprintf(output, "		{\n");
	fprintf(output, "0.00000 0.00000 1792486.48352 11217.41019 0.00000 26446.96585 62271.06470 0.00000 0.00000 0.00000 2357.68372 0.00000 0.00000 		}\n");
	fprintf(output, "		total=1894779.60797\n");
	fprintf(output, "		rangeLimitMax=0.50000\n");
	fprintf(output, "		rangeLimitMin=0.00000\n");
	fprintf(output, "		max_tax=22\n");
	fprintf(output, "		min_tax=21\n");
	fprintf(output, "	}\n");
	fprintf(output, "	poor_tax=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		current=0.11591\n");
	fprintf(output, "		tax_income=\n");
	fprintf(output, "		{\n");
	fprintf(output, "0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 133922.48761 2311114.97299 229248.16516 0.00000 0.00000 14694.64551 		}\n");
	fprintf(output, "		tax_eff=\n");
	fprintf(output, "		{\n");
	fprintf(output, "0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 368526.35513 5680316.87845 461141.38962 0.00000 0.00000 29381.26956 		}\n");
	fprintf(output, "		total=6539365.89276\n");
	fprintf(output, "		rangeLimitMax=0.50000\n");
	fprintf(output, "		rangeLimitMin=0.00000\n");
	fprintf(output, "		max_tax=22\n");
	fprintf(output, "		min_tax=21\n");
	fprintf(output, "	}\n");
	fprintf(output, "	education_spending=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		settings=1.00000\n");
	fprintf(output, "		factor=0.00998\n");
	fprintf(output, "		reserve=0.00000\n");
	fprintf(output, "		maxValue=233757.98865\n");
	fprintf(output, "		rangeLimitMax=1.00000\n");
	fprintf(output, "		rangeLimitMin=0.00000\n");
	fprintf(output, "		max_tax=0\n");
	fprintf(output, "		min_tax=0\n");
	fprintf(output, "	}\n");
	fprintf(output, "	crime_fighting=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		settings=0.79996\n");
	fprintf(output, "		factor=0.00998\n");
	fprintf(output, "		reserve=0.00000\n");
	fprintf(output, "		maxValue=98616.96771\n");
	fprintf(output, "		rangeLimitMax=1.00000\n");
	fprintf(output, "		rangeLimitMin=0.00000\n");
	fprintf(output, "		max_tax=0\n");
	fprintf(output, "		min_tax=0\n");
	fprintf(output, "	}\n");
	fprintf(output, "	social_spending=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		settings=1.00000\n");
	fprintf(output, "		factor=0.00998\n");
	fprintf(output, "		reserve=0.00000\n");
	fprintf(output, "		maxValue=11983.33038\n");
	fprintf(output, "		rangeLimitMax=1.00000\n");
	fprintf(output, "		rangeLimitMin=0.00000\n");
	fprintf(output, "		max_tax=0\n");
	fprintf(output, "		min_tax=25\n");
	fprintf(output, "	}\n");
	fprintf(output, "	military_spending=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		settings=0.29944\n");
	fprintf(output, "		factor=0.00998\n");
	fprintf(output, "		reserve=0.00000\n");
	fprintf(output, "		maxValue=280308.43228\n");
	fprintf(output, "		rangeLimitMax=0.75000\n");
	fprintf(output, "		rangeLimitMin=0.00000\n");
	fprintf(output, "		max_tax=24\n");
	fprintf(output, "		min_tax=23\n");
	fprintf(output, "	}\n");
	fprintf(output, "	overseas_penalty=0.00000\n");
	fprintf(output, "	leadership=2.97736\n");
	fprintf(output, "	auto_assign_leaders=yes\n");
	fprintf(output, "	auto_create_leaders=yes\n");
	fprintf(output, "}\n");
}


/*void outputRelations(FILE* output)
{
	ENG=
	{
		value=60
	}
	RUS=
	{
		value=-14
	}
	FRA=
	{
		value=-4
	}
	PRU=
	{
		value=0
	}
	AUS=
	{
		value=-15
	}
	USA=
	{
		value=0
	}
	JAP=
	{
		value=-10
		influence_value=0.004
	}
	DEN=
	{
		value=125
		level=3
		influence_value=25.006
	}
	SWE=
	{
		value=125
		level=3
		influence_value=30.006
	}
	BEL=
	{
		value=200
		level=5
		influence_value=25.006
	}
	NET=
	{
		value=125
		level=4
		influence_value=25.008
	}
	POR=
	{
		value=15
		influence_value=0.008
	}
	SPA=
	{
		value=-4
	}
	ANH=
	{
		value=20
		influence_value=0.006
	}
	BAD=
	{
		value=5
		influence_value=0.006
	}
	BAV=
	{
		value=5
		influence_value=0.006
	}
	BRA=
	{
		value=20
		influence_value=0.006
	}
	BRE=
	{
		value=20
		influence_value=0.006
	}
	COB=
	{
		value=20
		influence_value=0.006
	}
	FRM=
	{
		value=20
		influence_value=0.006
	}
	HAM=
	{
		value=20
		influence_value=0.006
	}
	HAN=
	{
		value=127
		level=5
		influence_value=25.006
	}
	HES=
	{
		value=20
		influence_value=0.006
	}
	HEK=
	{
		value=20
		influence_value=0.006
	}
	HOL=
	{
		value=20
		influence_value=0.006
	}
	LIP=
	{
		value=20
		influence_value=0.006
	}
	LUB=
	{
		value=20
		influence_value=0.006
	}
	LUX=
	{
		value=5
		influence_value=0.006
	}
	MEC=
	{
		value=20
		influence_value=0.006
	}
	MEI=
	{
		value=20
		influence_value=0.006
	}
	NAS=
	{
		value=20
		influence_value=0.006
	}
	OLD=
	{
		value=20
		influence_value=0.006
	}
	SAX=
	{
		value=20
		influence_value=0.006
	}
	WEI=
	{
		value=20
		influence_value=0.006
	}
	WUR=
	{
		value=5
		influence_value=0.006
	}
	LUC=
	{
		value=5
		influence_value=0.006
	}
	MOD=
	{
		value=5
		influence_value=0.006
	}
	PAP=
	{
		value=5
		influence_value=0.006
	}
	PAR=
	{
		value=5
		influence_value=0.006
	}
	SAR=
	{
		value=5
		influence_value=0.006
	}
	SIC=
	{
		value=5
		influence_value=0.006
	}
	TUS=
	{
		value=5
		influence_value=0.006
	}
	KRA=
	{
		value=5
		influence_value=0.006
	}
	SWI=
	{
		value=20
		influence_value=0.006
	}
	GRE=
	{
		value=5
		influence_value=0.006
	}
	ION=
	{
		value=107
		influence_value=0.006
	}
	MOL=
	{
		value=5
		influence_value=0.006
	}
	MON=
	{
		value=5
		influence_value=0.006
	}
	SER=
	{
		value=5
		influence_value=0.006
	}
	TUR=
	{
		value=-30
	}
	WAL=
	{
		value=5
		influence_value=0.006
	}
	ALD=
	{
		value=-10
		influence_value=0.004
	}
	EGY=
	{
		value=-10
		influence_value=0.004
	}
	ETH=
	{
		value=5
		influence_value=0.004
	}
	MAD=
	{
		value=5
		influence_value=0.004
	}
	LIB=
	{
		value=20
		influence_value=0.006
	}
	MOR=
	{
		value=-9
		influence_value=0.006
	}
	ORA=
	{
		value=20
		influence_value=0.006
	}
	SOK=
	{
		value=-10
		influence_value=0.004
	}
	TRI=
	{
		value=-10
		influence_value=0.004
	}
	TRN=
	{
		value=20
		influence_value=0.004
	}
	TUN=
	{
		value=-10
		influence_value=0.004
	}
	ZUL=
	{
		value=-9
		influence_value=0.006
	}
	ABU=
	{
		value=-10
		influence_value=0.004
	}
	AFG=
	{
		value=-10
		influence_value=0.004
	}
	BUK=
	{
		value=-10
		influence_value=0.004
	}
	HDJ=
	{
		value=-10
		influence_value=0.004
	}
	KHI=
	{
		value=-10
		influence_value=0.004
	}
	KOK=
	{
		value=-10
		influence_value=0.004
	}
	NEJ=
	{
		value=-10
		influence_value=0.004
	}
	OMA=
	{
		value=-10
		influence_value=0.004
	}
	PER=
	{
		value=-10
		influence_value=0.004
	}
	YEM=
	{
		value=-10
		influence_value=0.004
	}
	AWA=
	{
		value=127
		level=5
		influence_value=25.006
	}
	BAS=
	{
		value=127
		level=5
		influence_value=25.006
	}
	BER=
	{
		value=127
		level=5
		influence_value=25.006
	}
	BHO=
	{
		value=127
		level=5
		influence_value=25.006
	}
	BIK=
	{
		value=127
		level=5
		influence_value=25.006
	}
	BUN=
	{
		value=127
		level=5
		influence_value=25.006
	}
	GWA=
	{
		value=-9
		influence_value=0.006
	}
	HYD=
	{
		value=127
		level=5
		influence_value=25.006
	}
	IND=
	{
		value=127
		level=5
		influence_value=25.006
	}
	JAI=
	{
		value=127
		level=5
		influence_value=25.006
	}
	JAS=
	{
		value=127
		level=5
		influence_value=25.004
	}
	JOD=
	{
		value=127
		level=5
		influence_value=25.006
	}
	KAL=
	{
		value=-10
		influence_value=0.004
	}
	KUT=
	{
		value=-10
		influence_value=0.004
	}
	MAK=
	{
		value=-10
		influence_value=0.004
	}
	MEW=
	{
		value=127
		level=5
		influence_value=25.006
	}
	MYS=
	{
		value=127
		level=5
		influence_value=25.006
	}
	NAG=
	{
		value=127
		level=5
		influence_value=25.006
	}
	NEP=
	{
		value=-9
		influence_value=0.006
	}
	ORI=
	{
		value=127
		level=5
		influence_value=25.006
	}
	PAN=
	{
		value=-9
		influence_value=0.006
	}
	SIK=
	{
		value=-9
		influence_value=0.006
	}
	SIN=
	{
		value=-10
		influence_value=0.004
	}
	TRA=
	{
		value=127
		level=5
		influence_value=25.006
	}
	ATJ=
	{
		value=-10
		influence_value=0.004
	}
	BAL=
	{
		value=-10
		influence_value=0.004
	}
	BHU=
	{
		value=-9
		influence_value=0.006
	}
	BRU=
	{
		value=-10
		influence_value=0.004
	}
	BUR=
	{
		value=-9
		influence_value=0.006
	}
	CAM=
	{
		value=-10
		influence_value=0.004
	}
	CHI=
	{
		value=-10
		influence_value=0.001
	}
	DAI=
	{
		value=-10
		influence_value=0.004
	}
	KOR=
	{
		value=-10
		influence_value=0.004
	}
	LUA=
	{
		value=-10
		influence_value=0.004
	}
	JOH=
	{
		value=50
		level=3
		influence_value=0.006
	}
	SIA=
	{
		value=-9
		influence_value=0.006
	}
	TIB=
	{
		value=-9
		influence_value=0.006
	}
	HAI=
	{
		value=5
		influence_value=0.004
	}
	HAW=
	{
		value=20
		influence_value=0.004
	}
	MEX=
	{
		value=5
		influence_value=0.006
	}
	TEX=
	{
		value=20
		influence_value=0.004
	}
	UCA=
	{
		value=5
		influence_value=0.006
	}
	ARG=
	{
		value=125
		level=3
		influence_value=25.004
	}
	BOL=
	{
		value=5
		influence_value=0.004
	}
	BRZ=
	{
		value=15
		influence_value=0.006
	}
	CHL=
	{
		value=5
		influence_value=0.004
	}
	CLM=
	{
		value=5
		influence_value=0.004
	}
	ECU=
	{
		value=5
		influence_value=0.004
	}
	PEU=
	{
		value=5
		influence_value=0.004
	}
	PRG=
	{
		value=5
		influence_value=0.004
	}
	URU=
	{
		value=5
		influence_value=0.004
	}
	VNZ=
	{
		value=5
		influence_value=0.006
	}
}*/


void outputCountryMiddle(FILE* output)
{
	fprintf(output, "	active_inventions=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		316 250 64 65 66 76 77 78 79 85 86 87 88 89 90 122 123 124 242 243 244 245 246 247 248 249 262 263 264 265 300 301 312 313 314 315 316 357 358 359 	}\n");
	fprintf(output, "	illegal_inventions=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 67 68 69 70 71 72 73 74 75 80 81 82 83 84 91 92 93 94 95 96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120 121 125 126 127 128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143 144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159 160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191 192 193 194 195 196 197 198 199 200 201 202 203 204 205 206 207 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223 224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239 240 241 251 252 253 254 255 256 257 258 259 260 261 266 267 268 269 270 271 272 273 274 275 276 277 278 279 280 281 282 283 284 285 286 287 288 289 290 291 292 293 294 295 296 297 298 299 302 303 304 305 306 307 308 309 310 311 317 318 319 320 321 322 323 324 325 326 327 328 329 330 331 332 333 334 335 336 337 338 339 340 341 342 343 344 345 346 347 348 349 350 351 352 353 354 355 356 360 361 362 363 364 365 366 367 368 369 370 	}\n");
	fprintf(output, "	government_flag=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "	last_mission_cancel=\"1.1.1\"\n");
	fprintf(output, "	ai_hard_strategy=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "	schools=\"naval_tech_school\"\n");
	fprintf(output, "	primary_culture=\"british\"\n");
	fprintf(output, "	culture=\n");
	fprintf(output, "	{\n");
	fprintf(output, "\"anglo_canadian\"\n");
	fprintf(output, "	}\n");
	fprintf(output, "	prestige=100.052\n");
	fprintf(output, "	bank=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		money=240.00000\n");
	fprintf(output, "		money_lent=5.00217\n");
	fprintf(output, "	}\n");
	fprintf(output, "	money=75314.11652\n");
	fprintf(output, "	last_bankrupt=\"1.1.1\"\n");
	fprintf(output, "	creditor=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		country=\"---\"\n");
	fprintf(output, "		interest=0.01999\n");
	fprintf(output, "		debt=361.32385\n");
	fprintf(output, "		was_paid=yes\n");
	fprintf(output, "	}\n");
	fprintf(output, "	stockpile=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		small_arms=0.27832\n");
	fprintf(output, "		artillery=2.78528\n");
	fprintf(output, "		canned_food=1.68106\n");
	fprintf(output, "		clipper_convoy=7.94461\n");
	fprintf(output, "		steamer_convoy=2.32651\n");
	fprintf(output, "		wine=0.02930\n");
	fprintf(output, "	}\n");
	fprintf(output, "	nationalvalue=\"nv_order\"\n");
	fprintf(output, "	buy_domestic=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "	trade=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		small_arms=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.27832\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		artillery=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=8.76678\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		canned_food=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=1.68106\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		barrels=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		aeroplanes=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		cotton=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		dye=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		wool=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		silk=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		coal=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		sulphur=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		iron=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		timber=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		tropical_wood=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		rubber=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		oil=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		precious_metal=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		steel=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		cement=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		machine_parts=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		glass=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		fuel=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		fertilizer=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		explosives=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		clipper_convoy=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=17.16721\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		steamer_convoy=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=2.32651\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		electric_gear=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		fabric=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		lumber=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		paper=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		cattle=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		fish=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		fruit=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		grain=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		tobacco=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		tea=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		coffee=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		opium=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		automobiles=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		telephones=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		wine=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.02930\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		liquor=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		regular_clothes=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		luxury_clothes=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		furniture=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		luxury_furniture=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "		radio=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			limit=0.00000\n");
	fprintf(output, "			buy=yes\n");
	fprintf(output, "			automate_trade=yes\n");
	fprintf(output, "		}\n");
	fprintf(output, "	}\n");
	fprintf(output, "	civilized=yes\n");
	fprintf(output, "	last_greatness_date=\"1836.1.1\"\n");
}


void outputStateStuff(FILE* output)
{
	fprintf(output, "		is_colonial=yes\n");
	fprintf(output, "		savings=0.15726\n");
	fprintf(output, "		interest=0.00000\n");
	fprintf(output, "		popproject=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			input_goods=\n");
	fprintf(output, "			{\n");
	fprintf(output, "			}\n");
	fprintf(output, "			money=0.00000\n");
	fprintf(output, "			building=33\n");
	fprintf(output, "			province=14\n");
	fprintf(output, "			index=0\n");
	fprintf(output, "			type=1\n");
	fprintf(output, "			money2=yes\n");
	fprintf(output, "		}\n");
}


void outputCountryFooter(FILE* output)
{
	fprintf(output, "	badboy=0.000}\n");
	fprintf(output, "	tariffs=0.05002}\n");
	fprintf(output, "	max_tariff=2270.29584}\n");
	fprintf(output, "	domestic_supply_pool=}\n");
	fprintf(output, "	{}\n");
	fprintf(output, "		ammunition=3.29672}\n");
	fprintf(output, "		small_arms=0.30142}\n");
	fprintf(output, "		canned_food=7.92227}\n");
	fprintf(output, "		cotton=249.34164}\n");
	fprintf(output, "		dye=345.11856}\n");
	fprintf(output, "		wool=882.56940}\n");
	fprintf(output, "		iron=15.45239}\n");
	fprintf(output, "		timber=0.00003}\n");
	fprintf(output, "		machine_parts=2.15170}\n");
	fprintf(output, "		explosives=1.45895}\n");
	fprintf(output, "		steamer_convoy=3.72974}\n");
	fprintf(output, "		fabric=65.34714}\n");
	fprintf(output, "		cattle=55.14011}\n");
	fprintf(output, "		fish=876.88849}\n");
	fprintf(output, "		fruit=869.84662}\n");
	fprintf(output, "		grain=2191.73749}\n");
	fprintf(output, "		tobacco=386.75815}\n");
	fprintf(output, "		tea=436.90338}\n");
	fprintf(output, "		coffee=0.00003}\n");
	fprintf(output, "	}}\n");
	fprintf(output, "	sold_supply_pool=}\n");
	fprintf(output, "	{}\n");
	fprintf(output, "	}}\n");
	fprintf(output, "	domestic_demand_pool=}\n");
	fprintf(output, "	{}\n");
	fprintf(output, "		ammunition=2.79752}\n");
	fprintf(output, "		small_arms=0.47778}\n");
	fprintf(output, "		artillery=8.79233}\n");
	fprintf(output, "		canned_food=2.79990}\n");
	fprintf(output, "		cotton=297.45645}\n");
	fprintf(output, "		dye=32.38153}\n");
	fprintf(output, "		wool=281.10245}\n");
	fprintf(output, "		silk=4771.70380}\n");
	fprintf(output, "		coal=695.20877}\n");
	fprintf(output, "		sulphur=74.73886}\n");
	fprintf(output, "		iron=69.71292}\n");
	fprintf(output, "		timber=453.53769}\n");
	fprintf(output, "		tropical_wood=2527.91852}\n");
	fprintf(output, "		steel=379.74692}\n");
	fprintf(output, "		cement=16.78986}\n");
	fprintf(output, "		machine_parts=0.13553}\n");
	fprintf(output, "		glass=221.71170}\n");
	fprintf(output, "		fertilizer=4499.96829}\n");
	fprintf(output, "		explosives=2.46664}\n");
	fprintf(output, "		clipper_convoy=17.25189}\n");
	fprintf(output, "		steamer_convoy=2.32651}\n");
	fprintf(output, "		fabric=474.55545}\n");
	fprintf(output, "		lumber=620.83533}\n");
	fprintf(output, "		paper=7488.29086}\n");
	fprintf(output, "		cattle=191.08960}\n");
	fprintf(output, "		fish=271.44940}\n");
	fprintf(output, "		fruit=361.94940}\n");
	fprintf(output, "		grain=634.52951}\n");
	fprintf(output, "		tobacco=284.30426}\n");
	fprintf(output, "		tea=590.32074}\n");
	fprintf(output, "		coffee=147.36371}\n");
	fprintf(output, "		opium=4861.08102}\n");
	fprintf(output, "		wine=7488.41763}\n");
	fprintf(output, "		liquor=198.57391}\n");
	fprintf(output, "		regular_clothes=12117.97781}\n");
	fprintf(output, "		luxury_clothes=274.55560}\n");
	fprintf(output, "		furniture=4032.80402}\n");
	fprintf(output, "		luxury_furniture=274.55560}\n");
	fprintf(output, "	}}\n");
	fprintf(output, "	actual_sold_domestic=}\n");
	fprintf(output, "	{}\n");
	fprintf(output, "		ammunition=1.18616}\n");
	fprintf(output, "		small_arms=0.47778}\n");
	fprintf(output, "		artillery=1.22510}\n");
	fprintf(output, "		canned_food=2.79990}\n");
	fprintf(output, "		cotton=292.11496}\n");
	fprintf(output, "		dye=31.37018}\n");
	fprintf(output, "		wool=261.47684}\n");
	fprintf(output, "		coal=419.20801}\n");
	fprintf(output, "		sulphur=44.27762}\n");
	fprintf(output, "		iron=59.90646}\n");
	fprintf(output, "		timber=199.41193}\n");
	fprintf(output, "		tropical_wood=19.77554}\n");
	fprintf(output, "		steel=84.77158}\n");
	fprintf(output, "		cement=9.31128}\n");
	fprintf(output, "		machine_parts=0.11652}\n");
	fprintf(output, "		glass=183.97729}\n");
	fprintf(output, "		fertilizer=100.33215}\n");
	fprintf(output, "		explosives=1.49847}\n");
	fprintf(output, "		clipper_convoy=6.74332}\n");
	fprintf(output, "		steamer_convoy=2.32651}\n");
	fprintf(output, "		fabric=435.38019}\n");
	fprintf(output, "		lumber=161.59464}\n");
	fprintf(output, "		paper=299.20963}\n");
	fprintf(output, "		cattle=174.28638}\n");
	fprintf(output, "		fish=250.49490}\n");
	fprintf(output, "		fruit=339.37573}\n");
	fprintf(output, "		grain=587.41901}\n");
	fprintf(output, "		tobacco=259.76819}\n");
	fprintf(output, "		tea=543.80252}\n");
	fprintf(output, "		coffee=12.59415}\n");
	fprintf(output, "		opium=929.92966}\n");
	fprintf(output, "		wine=45.01218}\n");
	fprintf(output, "		liquor=44.32758}\n");
	fprintf(output, "		regular_clothes=206.13882}\n");
	fprintf(output, "		luxury_clothes=4.25790}\n");
	fprintf(output, "		furniture=224.44608}\n");
	fprintf(output, "		luxury_furniture=2.88101}\n");
	fprintf(output, "	}}\n");
	fprintf(output, "	saved_country_supply=}\n");
	fprintf(output, "	{}\n");
	fprintf(output, "		ammunition=3.13745}\n");
	fprintf(output, "		small_arms=0.77917}\n");
	fprintf(output, "		artillery=1.22510}\n");
	fprintf(output, "		canned_food=6.39532}\n");
	fprintf(output, "		cotton=145.93579}\n");
	fprintf(output, "		dye=356.23651}\n");
	fprintf(output, "		wool=911.79303}\n");
	fprintf(output, "		coal=349.55649}\n");
	fprintf(output, "		sulphur=25.38849}\n");
	fprintf(output, "		iron=48.69806}\n");
	fprintf(output, "		timber=24.22137}\n");
	fprintf(output, "		tropical_wood=19.77554}\n");
	fprintf(output, "		steel=57.76620}\n");
	fprintf(output, "		cement=7.04617}\n");
	fprintf(output, "		machine_parts=2.26822}\n");
	fprintf(output, "		glass=169.06641}\n");
	fprintf(output, "		fertilizer=99.35736}\n");
	fprintf(output, "		explosives=2.70624}\n");
	fprintf(output, "		clipper_convoy=6.73508}\n");
	fprintf(output, "		steamer_convoy=6.05624}\n");
	fprintf(output, "		fabric=385.71521}\n");
	fprintf(output, "		lumber=161.59464}\n");
	fprintf(output, "		paper=299.20963}\n");
	fprintf(output, "		cattle=201.52100}\n");
	fprintf(output, "		fish=1122.60907}\n");
	fprintf(output, "		fruit=1167.18854}\n");
	fprintf(output, "		grain=2698.88077}\n");
	fprintf(output, "		tobacco=646.52634}\n");
	fprintf(output, "		tea=871.22879}\n");
	fprintf(output, "		coffee=12.59418}\n");
	fprintf(output, "		opium=929.92966}\n");
	fprintf(output, "		wine=32.41226}\n");
	fprintf(output, "		liquor=32.87653}\n");
	fprintf(output, "		regular_clothes=123.19028}\n");
	fprintf(output, "		luxury_clothes=4.25790}\n");
	fprintf(output, "		furniture=224.44608}\n");
	fprintf(output, "		luxury_furniture=2.88101}\n");
	fprintf(output, "	}}\n");
	fprintf(output, "	max_bought=}\n");
	fprintf(output, "	{}\n");
	fprintf(output, "		small_arms=0.27832}\n");
	fprintf(output, "		artillery=2.78528}\n");
	fprintf(output, "		canned_food=1.68106}\n");
	fprintf(output, "		clipper_convoy=7.94461}\n");
	fprintf(output, "		steamer_convoy=2.32651}\n");
	fprintf(output, "		wine=0.02930}\n");
	fprintf(output, "	}}\n");
	fprintf(output, "	national_focus=}\n");
	fprintf(output, "	{}\n");
	fprintf(output, "	}}\n");
	fprintf(output, "	expenses=}\n");
	fprintf(output, "	{}\n");
	fprintf(output, "116.87897 49.30847 5.99164 140.15421 0.00000 0.25754 686.47058 0.00000 0.00000 0.00000 0.00000 	}}\n");
	fprintf(output, "	incomes=}\n");
	fprintf(output, "	{}\n");
	fprintf(output, "3269.68292 947.38977 559.46048 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 	}}\n");
}