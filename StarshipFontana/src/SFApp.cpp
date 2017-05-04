#include "SFApp.h"
int score=0;

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w/2, 22);
  player->SetPosition(player_pos);

  const int number_of_aliens = 3;
  for(int i=0; i<number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2((canvas_w/number_of_aliens) * i+100, 300.0f);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }

  const int number_of_walls = 3;
    for(int i=0; i<number_of_walls; i++) {
      // place an walls at width/number_of_walls * i
      auto wall = make_shared<SFAsset>(SFASSET_WALL, sf_window);
      auto pos   = Point2((canvas_w/number_of_walls) * i+70, 200.0f);
      wall->SetPosition(pos);
      walls.push_back(wall);
  }

  auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
  auto pos  = Point2((canvas_w/4), 400);
  coin->SetPosition(pos);
  coins.push_back(coin);
}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWest();
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEast();
    break;
  case SFEVENT_PLAYER_UP:
	  player->GoNorth();
	  break;
  case SFEVENT_PLAYER_DOWN:
	  player->GoSouth();
	  break;
  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // wrap an SDL_Event with our SFEvent
    SFEvent sfevent((const SDL_Event) event);
    // handle our SFEvent
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {
  // Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();
  }

  for(auto c: coins) {
   // c->GoNorth();
  }

  // Update enemy positions
  for(auto a : aliens) {
    // do something here
  }

  // Detect collisions
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
        p->HandleCollision();
        a->HandleCollision();
        score++;
        cout << score << endl;
      }
    }
  }

  // Player colliding with alien
  for(auto a: aliens) {
	if(player->CollidesWith(a)) {
		player->SetNotAlive();
		a->HandleCollision();
		cout << "GameOver" << endl;
	}
  }

  //Player colliding with wall

  for(auto w: walls) {
	  if(player->CollidesWith(w)) {
		  player->SetNotAlive();
		  list<shared_ptr<SFAsset>> tmp;
		  tmp.push_back(player);
		  cout << "GameOver" << endl;
		  tmp.clear();
	  }
  }

  //Collect Coin
  for(auto c: coins) {
	  if(player->CollidesWith(c)) {
		  c->SetNotAlive();
		  cout << "YOU WIN! You collected the MEGA COIN, you're now so rich you can stop playing this game" << endl;
	  }
  }

/*  // Remove Coin
  list<shared_ptr<SFAsset>> tmp;
  for(auto c: coins) {
	  if(c->IsAlive()) {
		  tmp.push_back(c);
	  }
  }
*/


  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);
}

void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());

  // draw the player
  if (player->IsAlive()) {
	  player->OnRender();
  }

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender();}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {
      a->OnRender();
    }
  }

  for(auto c: coins) {
	  if(c->IsAlive()) {
		c->OnRender();
	  }
  }


  for(auto w: walls) {
	w->OnRender();
  }

  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
