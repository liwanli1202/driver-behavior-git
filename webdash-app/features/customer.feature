Feature: Customer

  In order to let customers view their dashboards, I need customer functionality

  Scenario: View webdash

    A customer should be able to log in and view the real-time web dashboard

    Given I am a customer
    When I visit the main page
    Then I should see a login form
    When I submit the login form
    Then I should see my dashboard
    And I should see a Sign out link
    When I click the Sign out link
    Then I should see a login form

  @wip
  Scenario: View real time web dashboard update

    A customer should be able to see the real time web dashboard update while vehicle is running

    Given I am a customer
    And I am signed in
    And there is a vehicle with a route
    When I visit the dashboard
    Then I should see a link for the route
    When I click on the link for the route
    Then I should see the live status page for the route
    When the vehicle starts
    Then I should see the live status changing
