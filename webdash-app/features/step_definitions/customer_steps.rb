
def submit_login_form
  fill_in 'Email', with: @user.email
  fill_in 'Password', with: @user.password
  click_button 'Sign in'
end

Given(/^I am a customer$/) do
  @user = FactoryGirl.create :customer
end

Given(/^I am signed in$/) do
  visit '/users/sign_in'
  submit_login_form
end

Given(/^there is a vehicle with a route$/) do
  @vehicle = FactoryGirl.create :vehicle
  @route = FactoryGirl.create :route
  @vehicle_route = FactoryGirl.create :vehicle_route, vehicle: @vehicle, route: @route
end

When(/^I visit (.*)$/) do |page|
  case page
    when 'the main page'
      visit root_url
    when 'the dashboard'
      visit root_url
    else
      raise 'Unexpected page description'
  end
end

Then(/^I should see a login form$/) do
  expect(page).to have_selector('form input#user_email')
  expect(page).to have_selector('form input#user_password')
end

When(/^I submit the login form$/) do
  submit_login_form
end

Then(/^I should see my dashboard$/) do
  expect(page).to have_text 'Dashboard'
  expect(page).to have_text 'Alerts'
  expect(page).to have_text 'Current routes'
end

Then(/^I should see a (.*) link$/) do |link_text|
  expect(page).to have_link link_text
end

When(/^I click the (.*) link$/) do |link_text|
  click_link link_text
end

Then(/^I should see a link for the route$/) do
  expect(page).to have_selector('a', text: /#{@route.name}.*#{@vehicle.vehicle_id}.*#{@vehicle_route.start_time}.*#{@vehicle_route.finish_time}/)
end

When(/^I click on the link for the route$/) do
  click_link @vehicle_route.name
end

Then(/^I should see the live status page for the route$/) do
  expect(page).to have_text 'Route Status'
  expect(page).to have_text @route.name
  expect(page).to have_text @vehicle.vehicle_id
  expect(page).to have_text @vehicle_route.start_time
  expect(page).to have_text @vehicle_route.finish_time
  expect(page).to have_selector 'div.demo-container div#placeholder'
  expect(page).to have_selector 'div.demo-container div#barholder'
end

When(/^the vehicle starts$/) do
  fail 'cannot execute fake vehicle connector' unless system('bundle exec rake fake_vehicle_connector')
end

Then(/^I should see the live status changing$/) do
  fail "don't know how to tell if the live status is changing yet"
end
