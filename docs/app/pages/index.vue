<template>
  <UContainer class="grid place-items-center">
    <section class="grid place-content-center text-center h-screen">
      <h1 class="text-6xl pb-6">Setup your Wordclock</h1>
      <div class="flex justify-center gap-4">
        <InstallButton manifest="esp-manifest.json" />
        <UButton
          class="rounded-full font-bold px-6 py-3"
          variant="outline"
          color="secondary"
          label="Tutorial"
          href="#tutorial"
        />
      </div>
    </section>

    <USeparator />

    <section id="tutorial" class="min-h-screen w-full py-8">
      <h2 class="text-3xl">How the setup works</h2>

      <h3 class="text-xl pt-6">Connect to your wordclock</h3>
      <p>
        First make sure to connect your wordclock to your computer via USB.
        Afterwards open this website in a web browser supporting the Web Serial
        API. (For example Google Chrome or Microsoft Edge)
      </p>
      <p>
        Once ready, click the green "Connect" button on the top of this website.
        A Dialog will open asking you for the device you want to connect to:
      </p>
      <NuxtImg class="py-4" src="connect-to-port.png" />
      <p class="whitespace-pre-wrap">
        Select your wordclock from the list. In case you're unsure which device
        to pick, click the connect button once without and once with it being
        connected.
      </p>

      <h3 class="text-xl pt-6">Install the software</h3>
      <p>
        Once connected a new dialog will prompt you to install the latest
        software onto your wordclock.
      </p>
      <NuxtImg class="py-4" src="install.png" />
      <p>
        Click the "Install Wordclock" button. When being asked whether you want
        to erase the device feel free to select the checkbox if you want to do a
        clean install. Continue with the installation and wait until it
        finished.
      </p>
      <NuxtImg class="py-4" src="loading.png" />

      <h3 class="text-xl pt-6">Configure Wifi</h3>
      <p>
        After the installation finished you'll be greeted with a success window,
        followed by a question whether you want to configure the WiFi settings
        of your Wordclock.
      </p>
      <NuxtImg class="py-4" src="connect-wifi-prompt.png" />
      <p>
        Click "Connect to Wi-Fi" and wait for it to scan for available networks.
      </p>
      <p>
        Once finished, either select one of the found networks and enter your
        password or try re-scanning using the icon in the top right corner. If
        your network couldn't be found, you can also choose "Join other..." and
        enter the name and password for your network.
      </p>
      <NuxtImg class="py-4" src="wifi-setup.png" />
      <p>
        Once successfully connected to wifi you'll be able to visit the
        dashboard of your wordclock. Just click "Visit Device" and you'll be
        redirected to
        <a href="http://wordclock.local/">http://wordclock.local/</a>.
      </p>

      <p>
        <b>Note:</b> It might take a moment until the dashboard is available.
        Should the page not load immediately, just wait a minute and then try
        reloading it.
      </p>
      <NuxtImg class="py-4" src="visit-device.png" />
    </section>

    <USeparator />

    <section class="w-full py-8">
      <h2 class="text-3xl">Trouble shooting</h2>

      <h3 class="text-xl pt-6">
        <a href="http://wordclock.local/">http://wordclock.local/</a> could not
        be opened
      </h3>
      <p>
        As mentioned above it can take a moment after installation until the
        dashboard starts up. It will also take a few seconds until the URL
        <a href="http://wordclock.local/">http://wordclock.local/</a> is
        available depending on your local network.
      </p>
      <p>
        In case it's still not available after 5 minutes, please check your
        routers settings whether the "*.local" domain was changed e.g. to
        something else.
      </p>
      <p>
        If the domain still doesn't work, you'll have to connect to the
        dashboard using the IP address of your wordclock. Check your router to
        find it.
      </p>

      <h3 class="text-xl pt-6">Connecting to wifi isn't possible</h3>
      <p>
        In case setting up wifi via the web installer doesn't work, you can
        always connect to your wordclock directly.
      </p>
      <p>
        If no wifi connection is setup the wordclock will start its own access
        point. Just connect to the wifi network "Wordclock" and visit
        <a href="http://wordclock.local/">http://wordclock.local/</a> (or
        <a href="http://192.168.4.1/">http://192.168.4.1/</a>). You can setup
        your wifi connection from there as well.
      </p>

      <h3 class="text-xl pt-6">Last resort</h3>
      <p>
        If none of the solutions mentioned above worked for you, there's always
        the last resort of restarting the wordclock or even re-installing the
        software. You can always re-connect to your wordclock using the
        "Connect" button above to erase and re-install it.
      </p>
      <NuxtImg class="py-4" src="last-resort-meme.png" />
    </section>
  </UContainer>
</template>
