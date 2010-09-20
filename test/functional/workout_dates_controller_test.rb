require 'test_helper'

class WorkoutDatesControllerTest < ActionController::TestCase
  setup do
    @workout_date = workout_dates(:one)
  end

  test "should get index" do
    get :index
    assert_response :success
    assert_not_nil assigns(:workout_dates)
  end

  test "should get new" do
    get :new
    assert_response :success
  end

  test "should create workout_date" do
    assert_difference('WorkoutDate.count') do
      post :create, :workout_date => @workout_date.attributes
    end

    assert_redirected_to workout_date_path(assigns(:workout_date))
  end

  test "should show workout_date" do
    get :show, :id => @workout_date.to_param
    assert_response :success
  end

  test "should get edit" do
    get :edit, :id => @workout_date.to_param
    assert_response :success
  end

  test "should update workout_date" do
    put :update, :id => @workout_date.to_param, :workout_date => @workout_date.attributes
    assert_redirected_to workout_date_path(assigns(:workout_date))
  end

  test "should destroy workout_date" do
    assert_difference('WorkoutDate.count', -1) do
      delete :destroy, :id => @workout_date.to_param
    end

    assert_redirected_to workout_dates_path
  end
end
