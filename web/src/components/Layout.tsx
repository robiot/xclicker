import React, { useEffect } from "react";
import Head from "next/head";

export function Layout({ children, title, home = false }) {
  return (
    <div className="min-h-screen bg-gray-900 text-white">
      <Head>
        <title>{home ? `XClicker - ${title}` : `${title} - XClicker`}</title>
        <meta charSet="utf-8" />
        <meta name="viewport" content="initial-scale=1.0, width=device-width" />
        <meta name="description" content="A fast gui autoclicker for linux" title="XClicker" />
        <meta property="og:url" content="https://xclicker.pages.dev" />
        <meta property="og:image" content="/embed.png" />

        <meta property="og:image" content="/embed.png" />
        <meta property="og:image:alt" content="A fast gui autoclicker for linux." />
        <meta property="og:site_name" content="GitHub" />
        <meta property="og:type" content="profile" />
        <meta property="og:title" content={`XClicker - ${title}`} />
        <meta property="og:url" content="https://xclicker.pages.dev" />
        <meta property="og:description" content="A fast gui autoclicker for linux." />

        <link rel="icon" href="/favicon.ico" />
      </Head>

      {children}
    </div>
  );
}

export default Layout;
