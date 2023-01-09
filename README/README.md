<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a name="readme-top"></a>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![MIT License][license-shield]][license-url]

<!-- PROJECT LOGO -->
<br />
<div align="center">
<h2 align="center">GOAP Survival Agent</h2>

  <p align="center">
    Making an AI agent survive a virtual wasteland using Goal-Oriented Action Planning.
    <br />
    <a href="https://github.com/belmansjef/GOAP_ZombieGame"><strong>View demo »</strong></a>
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#what-is-goap">What Is GOAP?</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->
## About The Project

This project implements <a href="https://alumni.media.mit.edu/~jorkin/goap.html">Jeff Orkin's Goal-Oriented Action Planning</a> to help an AI agent survive a virtual 2D wasteland plagued with zombies coming for you (well, your brains to be more precise, if classic comics are to be believed).

Writen in C++, using the in-house engine developed by a few talented <a href="https://www.digitalartsandentertainment.be/">DAE</a> profs.

### Built With

[![cpp][cpp]][cpp-url]
<!-- What Is GOAP -->
<a name="what-is-goap"></a>
## What is GOAP?

If you were to ask a few hundred game developers to write down the most common way to implement AI in games, <a href="https://en.wikipedia.org/wiki/Finite-state_machine">Finite State Machines</a> (FSM) and <a href="https://en.wikipedia.org/wiki/Behavior_tree">Behaviour Trees</a> (BT) would surely top the charts. Almost every gameplay programmer has used, or at least heard of, Finite State Machines.

Goal-Oriented Action Planing (GOAP, rhymes with dope) is a planning architecture based on <a href="http://web.mit.edu/caelan/www/publications/rss_workshop_2017.pdf">MIT's STRIPS planning</a>. Developed by <a href="http://alumni.media.mit.edu/~jorkin/">Jeff Orkin</a> while he was working at <a href="https://www.lith.com/">Monolith Productions</a>. GOAP was first implemented in <a href="https://en.wikipedia.org/wiki/F.E.A.R.">F.E.A.R.</a>, which back in 2005 was praised for it's intelligent AI and squad behaviour in which agents could seemingly verbally communicate with eachother (although the latter was all <a href="https://alumni.media.mit.edu/~jorkin/gdc2006_orkin_jeff_fear.pdf#page=16">smoke and mirrors</a>).

Compared to a traditional FSM where you would need to define every single transition between every single state, which gets tedious and cumbersome real fast as the number of states/actions grow, GOAP's FSM only ever uses three states. But how could a FSM with only three states navigate an agent through a wasteland whilst fighting of zombies, making sure to avoid starvation, I hear you ask? This is where the planning comes in to play. By giving the <a href="actions">planner</a> a list of <a href="actions">actions</a>, the current <a href="states">state</a> of the world and a desired world state, it will compute a sequence of actions to be taken to achieve said desired world state.

<a name="actions"></a>
### Actions

This is an example of how to list things you need to use the software and how to install them.
* npm
  ```sh
  npm install npm@latest -g
  ```
<a name="states"></a>
### States

<a name="planner"></a>
### Planner


<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage

Use this space to show useful examples of how a project can be used. Additional screenshots, code examples and demos work well in this space. You may also link to more resources.

_For more examples, please refer to the [Documentation](https://example.com)_

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

- [ ] Feature 1
- [ ] Feature 2
- [ ] Feature 3
    - [ ] Nested Feature

See the [open issues](https://github.com/belmansjef/GOAP_ZombieGame/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the GNU General Public License v3.0. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

Your Name - [@twitter_handle](https://twitter.com/twitter_handle) - email@email_client.com

Project Link: [https://github.com/belmansjef/GOAP_ZombieGame](https://github.com/belmansjef/GOAP_ZombieGame)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* []()
* []()
* []()

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/belmansjef/GOAP_ZombieGame.svg?style=for-the-badge
[contributors-url]: https://github.com/belmansjef/GOAP_ZombieGame/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/belmansjef/GOAP_ZombieGame.svg?style=for-the-badge
[forks-url]: https://github.com/belmansjef/GOAP_ZombieGame/network/members
[stars-shield]: https://img.shields.io/github/stars/belmansjef/GOAP_ZombieGame.svg?style=for-the-badge
[stars-url]: https://github.com/belmansjef/GOAP_ZombieGame/stargazers
[issues-shield]: https://img.shields.io/github/issues/belmansjef/GOAP_ZombieGame.svg?style=for-the-badge
[issues-url]: https://github.com/belmansjef/GOAP_ZombieGame/issues
[license-shield]: https://img.shields.io/github/license/belmansjef/GOAP_ZombieGame.svg?style=for-the-badge
[license-url]: https://github.com/belmansjef/GOAP_ZombieGame/blob/main/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/linkedin_username
[product-screenshot]: images/screenshot.png
[cpp]: https://img.shields.io/badge/CPlusPlus-000000?style=for-the-badge&logo=cplusplus&logoColor=white
[cpp-url]: https://en.cppreference.com/w/cpp/language
[React.js]: https://img.shields.io/badge/React-20232A?style=for-the-badge&logo=react&logoColor=61DAFB
[React-url]: https://reactjs.org/
[Vue.js]: https://img.shields.io/badge/Vue.js-35495E?style=for-the-badge&logo=vuedotjs&logoColor=4FC08D
[Vue-url]: https://vuejs.org/
[Angular.io]: https://img.shields.io/badge/Angular-DD0031?style=for-the-badge&logo=angular&logoColor=white
[Angular-url]: https://angular.io/
[Svelte.dev]: https://img.shields.io/badge/Svelte-4A4A55?style=for-the-badge&logo=svelte&logoColor=FF3E00
[Svelte-url]: https://svelte.dev/
[Laravel.com]: https://img.shields.io/badge/Laravel-FF2D20?style=for-the-badge&logo=laravel&logoColor=white
[Laravel-url]: https://laravel.com
[Bootstrap.com]: https://img.shields.io/badge/Bootstrap-563D7C?style=for-the-badge&logo=bootstrap&logoColor=white
[Bootstrap-url]: https://getbootstrap.com
[JQuery.com]: https://img.shields.io/badge/jQuery-0769AD?style=for-the-badge&logo=jquery&logoColor=white
[JQuery-url]: https://jquery.com 

# GOAP_ZombieGame
Research done on GOAP (Goal Oriented Action Planning) applied to a zombie game developed using an in-house engine of DAE, a course at Howest, Belgium.

## SOURCES ##
Websites:

https://alumni.media.mit.edu/~jorkin/goap.html
https://jaredemitchell.com/goal-oriented-action-planning-research/
https://medium.com/@vedantchaudhari/goal-oriented-action-planning-34035ed40d0b
https://gamedevelopment.tutsplus.com/tutorials/goal-oriented-action-planning-for-a-smarter-ai--cms-20793

Github:
https://github.com/crashkonijn/GOAP

Videos:
https://www.youtube.com/watch?v=Q7aHXn_LypI
https://www.youtube.com/watch?v=gm7K68663rA
