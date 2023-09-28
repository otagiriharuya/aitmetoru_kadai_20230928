# include <Siv3D.hpp>

// アイテムの情報
struct ItemInfo
{
	// アイテムのテクスチャ
	Texture texture;

	// 落下速度（ピクセル / 秒）
	double speed;

	// 得点
	int32 score;

	//得点減らす
	int32 fallscore;
};

// フィールド上のアイテム
struct Item
{
	// アイテムの種類
	int32 type;

	// アイテムの現在位置
	Vec2 pos;
};

void Main()
{
	// プレイヤーの絵文字テクスチャ
	const Texture playerTexture{ U"👨‍🦼"_emoji };

	// スコア表示用のフォント
	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };

	// プレイヤーのスピード（ピクセル / 秒)
	constexpr double PlayerSpeed = 800.0;

	// アイテムが発生する時間間隔（秒）
	constexpr double ItemSpawnInterval = 0.4;

	// アイテムのあたり判定の円の半径（ピクセル）
	constexpr double ItemRadius = 40.0;

	// アイテムのテクスチャ
	const Array<ItemInfo> ItemInfos =
	{
		//          絵文字　　　スピード　+スコア -スコア
		{ Texture{ U"🍮"_emoji }, 200.0, 50, 800},
		{ Texture{ U"🍫"_emoji }, 300.0, 200, 500},
		{ Texture{ U"🍦"_emoji }, 500.0, 500, 300},
		{ Texture{ U"🍰"_emoji }, 600.0, 800, 100},
		{ Texture{ U"🍡"_emoji }, 700.0, 1000, 10},
		{ Texture{ U"🍺"_emoji }, 400.0, -10000, 1000},
	};

	// 最後にアイテムが発生してからの経過時間（秒）
	double itemSpawnAccumulatedTime = 0.0;

	// プレイヤーの座標
	Vec2 playerPos{ 400, 500 };

	// 現在画面上にあるアイテムの配列
	Array<Item> items;

	// スコア
	int32 score = 0;

	//乱数
	int32 r=0;

	//タイプ
	int32 type;

	//プレイヤーが右を向いているか
	bool isPlayerFacingRight = true;

	while (System::Update())
	{
		////////////////////////////////
		//
		//	状態更新
		//
		////////////////////////////////

		// 前のフレームからの経過時間 (秒)
		const double deltaTime = Scene::DeltaTime();

		// プレイヤーの移動に関する処理
		{
			if (KeyLeft.pressed()) // [←] キーが押されていたら
			{
				playerPos.x -= (PlayerSpeed * deltaTime);
				isPlayerFacingRight = false;
			}
			else if (KeyRight.pressed()) // [→] キーが押されていたら
			{
				playerPos.x += (PlayerSpeed * deltaTime);
				isPlayerFacingRight = true;
			}

			// 壁の外に出ないようにする
			// Clamp(x, min, max) は, x を min～max の範囲に収めた値を返す
			playerPos.x = Clamp(playerPos.x, 0.0, 800.0);
		}

		// アイテムの出現と移動と消滅に関する処理
		{
			itemSpawnAccumulatedTime += deltaTime;

			// spawnTime が経過するごとに新しいアイテムを出現させる
			while (ItemSpawnInterval <= itemSpawnAccumulatedTime)
			{
				r =  Random(0, 2);
				if (r == 0) {
					type = (RandomBool(0.5) ? 0 : 1);
				}
				if (r == 1) {
					type = (RandomBool(0.7) ? 2 : 3);
				}
				if (r == 2) {
					type = (RandomBool(0.9) ? 5 : 4);
				}
				// 新しく出現するアイテムを配列に追加する
				items << Item
				{
					//.type = (Random(5)),// アイテムの種類
					.type = type,
					.pos = { Random(100, 700), -100 }, // アイテムの初期座標
				};

				itemSpawnAccumulatedTime -= ItemSpawnInterval;
			}

			// すべてのアイテムについて移動処理を行う
			for (auto& item : items)
			{
				item.pos.y += (ItemInfos[item.type].speed * deltaTime);
			}

			// プレイヤーのあたり判定の円
			const Circle playerCircle{ playerPos, 60 };

			// アイテムのあたり判定と回収したアイテムの削除
			for (auto it = items.begin(); it != items.end();)
			{
				// アイテムのあたり判定の円
				const Circle itemCircle{ it->pos, ItemRadius };

				// 交差したらアイテムを削除
				if (playerCircle.intersects(itemCircle))
				{
					// (削除する前に) スコアを加算する
					score += ItemInfos[it->type].score;

					// アイテムを削除する
					it = items.erase(it);
				}
				else
				{
					// イテレータを次のアイテムに進める
					++it;
				}
			}

			// 画面外に出たアイテムを消去する
			items.remove_if([](const Item& item) { return (700 < item.pos.y); });
		}

		////////////////////////////////
		//
		//	描画
		//
		////////////////////////////////

		// 背景を描画する
		Scene::Rect().draw(Arg::top = ColorF{ 0.1, 0.4, 0.8 }, Arg::bottom = ColorF{ 0.3, 0.7, 1.0 });

		//円を描画する　x,y,r　　　　　　　色を変える
		Circle{ 800, 0, 100 }.draw(ColorF{ 1.0, 0.5, 0.0 });

		// 地面を描画する
		Rect{ Arg::bottomLeft(0, Scene::Height()), Scene::Width(), 60 }.draw(ColorF{ 0.2, 0.6, 0.3 });

		// プレイヤーのテクスチャを描画する
		playerTexture.mirrored(isPlayerFacingRight).drawAt(playerPos);

		// アイテムを描画する
		for (const auto& item : items)
		{
			ItemInfos[item.type].texture.resized(ItemRadius * 2).drawAt(item.pos);
		}

		//円を描画する　x,y,r　　　　　　　色を変える
		Circle{ 800, 0, 100 }.draw(ColorF{ 1.0, 0.5, 0.0 });

		// スコアを描画する
		font(ThousandsSeparate(score)).draw(30, Vec2{ 20, 20 });
	}
}
