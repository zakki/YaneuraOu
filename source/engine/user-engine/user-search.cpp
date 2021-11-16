#include "../../types.h"
#include "../../extra/all.h"

#include <set>

#if defined(USER_ENGINE)

void uni_search2(Position& pos, StateInfo* si__, Thread* th)
{
  ExtMove moves[MAX_MOVES];
  std::vector<PackedSfen> sfens;
  std::set<HASH_KEY> sfensUniq;
  std::map<HASH_KEY, HASH_KEY> allParents;
  std::map<HASH_KEY, PackedSfen> allSfen;

  StateInfo si;
  PackedSfen hirate;
  pos.set_hirate(&si, th);
  pos.sfen_pack(hirate);
  sfens.push_back(hirate);
  sfensUniq.insert(si.board_long_key());

  std::vector<StateInfo> states;
  states.resize(2);

  for (int depth = 0;; depth++) {
    std::map<HASH_KEY, int64_t> count;
    std::map<HASH_KEY, int> lance;
    std::map<HASH_KEY, PackedSfen> nextSfens;
    std::map<HASH_KEY, HASH_KEY> parents;
    for (auto& sfen : sfens) {
      //pos.set(sfen, &si, th);
	  pos.set_from_packed_sfen(sfen, &si, th);
      if (depth > 0)
        pos.do_null_move(states[0]);
      //cout << sfen << "---------------------------------------------------------------------------------------------" << std::endl;
      //cout << pos;
	  auto parent_key = si.board_long_key();
      auto parent = 2 - sfensUniq.count(parent_key);
      auto endMove = generateMoves<LEGAL_ALL>(pos, moves);
      for (auto m = moves; m != endMove; m++) {
        auto piece = type_of(pos.moved_piece_before(m->move));
        if (piece == PieceType::PAWN
          //|| piece == Piece::LANCE
          )
          continue;
        pos.do_move(*m, states[1]);
        //pos.set_state(si);
		auto key = states[1].board_long_key();
        count[key] += parent;
        if (piece == PieceType::LANCE)
          lance[key]++;
        //if (count[key] == 1) {
		if (nextSfens.find(key) == nextSfens.end()) {
		  PackedSfen s;
		  pos.sfen_pack(s);
		  nextSfens[key] = s;
		  parents[key] = parent_key;
		  allSfen[key] = s;
		}
        //}
        pos.undo_move(*m);
      }
    }
    std::vector<PackedSfen> nextSfensAll;
    std::set<HASH_KEY> nextSfensUniq;
    int show = 0;
    cout << "DEPTH " << depth << "*********************************************************************************************" << std::endl;
    for (auto& k : count) {
      nextSfensAll.push_back(nextSfens[k.first]);
      if (k.second != 1) {
        continue;
      }
      auto sfen = nextSfens[k.first];
      nextSfensUniq.insert(k.first);
      allParents[k.first] = parents[k.first];
      pos.set_from_packed_sfen(sfen, &si, th);
      if (type_of(pos.piece_on(SQ_11)) != PieceType::LANCE || type_of(pos.piece_on(SQ_19)) != PieceType::LANCE
        || type_of(pos.piece_on(SQ_91)) != PieceType::LANCE || type_of(pos.piece_on(SQ_99)) != PieceType::LANCE) {
        cout << "LANCE" << std::endl;
        cout << pos;
      } else if (show < 1000) {
        cout << pos;
        show++;
      }
      if (show < 5) {
        cout << "========================================================================================" << std::endl;
        for (auto s = k.first; allParents.find(s) != allParents.end(); s = allParents[s]) {
          pos.set_from_packed_sfen(allSfen[s], &si, th);
          cout << pos;
        }
        cout << "========================================================================================" << std::endl;
      }
    }
    cout << nextSfensUniq.size() << "/" << nextSfensAll.size() << std::endl;
    sfens = nextSfensAll;
    sfensUniq = nextSfensUniq;
	if (nextSfensUniq.size() == 0)
		return;
  }
}

// USI拡張コマンド"user"が送られてくるとこの関数が呼び出される。実験に使ってください。
void user_test(Position& pos_, std::istringstream& is)
{
  StateInfo si;

  pos_.set_hirate(&si, Threads.main());
  pos_.do_move(make_move(SQ_17, SQ_16, Piece::B_PAWN), si);
  pos_.do_null_move(si);
  pos_.do_move(make_move(SQ_27, SQ_26, Piece::B_PAWN), si);
  cout << pos_;
  cout << pos_.key() << std::endl;

  pos_.set_hirate(&si, Threads.main());
  pos_.do_move(make_move(SQ_27, SQ_26, Piece::B_PAWN), si);
  pos_.do_null_move(si);
  pos_.do_move(make_move(SQ_17, SQ_16, Piece::B_PAWN), si);
  cout << pos_;
  cout << pos_.key() << std::endl;

  pos_.set_hirate(&si, Threads.main());
  cout << pos_;
  std::vector<std::string> init = { SFEN_HIRATE };
  uni_search2(pos_, &si, Threads.main());
}

// USIに追加オプションを設定したいときは、この関数を定義すること。
// USI::init()のなかからコールバックされる。
void USI::extra_option(USI::OptionsMap & o)
{
}

// 起動時に呼び出される。時間のかからない探索関係の初期化処理はここに書くこと。
void Search::init()
{
}

// isreadyコマンドの応答中に呼び出される。時間のかかる処理はここに書くこと。
void  Search::clear()
{
}

// 探索開始時に呼び出される。
// この関数内で初期化を終わらせ、slaveスレッドを起動してThread::search()を呼び出す。
// そのあとslaveスレッドを終了させ、ベストな指し手を返すこと。
void MainThread::search()
{
  // 例)
  //  for (auto th : Threads.slaves) th->start_searching();
  //  Thread::search();
  //  for (auto th : Threads.slaves) th->wait_for_search_finished();
}

// 探索本体。並列化している場合、ここがslaveのエントリーポイント。
void Thread::search()
{
}

#endif // USER_ENGINE
