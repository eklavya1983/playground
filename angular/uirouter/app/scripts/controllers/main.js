'use strict';

/**
 * @ngdoc function
 * @name angularExApp.controller:MainCtrl
 * @description
 * # MainCtrl
 * Controller of the angularExApp
 */
angular.module('angularExApp')
  .controller('MainCtrl', function ($scope) {
    $scope.awesomeThings = [
      'HTML5 Boilerplate',
      'AngularJS',
      'Karma'
    ];
  });
